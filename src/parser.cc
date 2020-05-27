#include "parser.h"

#include <cassert>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cstdint>
#include <iostream>


using namespace diags::parsing;

Parser::Parser(std::istream& inputStreamParam)
	: buffer{}, inputStream(inputStreamParam) {}

std::vector<Diagnostic> Parser::parse() {
	read(BUFFER_SIZE);
	return readOutput();
}

// Этот символ никогда не должен встречаться во входном файле.
// Вывод GCC всё-таки читает пользователь, потому вряд ли встретится нулевой байт,
// который обычно непечатаемый (в ASCII). Даже если применить это только к выводу сообщений
// компилятора (мы можем так сделать, потому что считаем, что пользователь вызывает компилятор
// с локалью "C", которая, как мне кажется, говорит компилятору использовать сообщения на
// английском языке, выводить текст в кодировке ASCII), но не к вставкам кода пользователя, то
// пользователь вряд ли вставит в свой код нулевой байт: в строках его надо экранировать,
// вне строк, я думаю, всегда будет синтаксической ошибкой.
// Можем пройтись по файлу один раз, чтобы убедиться, что такого символа нет.
const char Parser::END_OF_FILE = 0;

// Не должно получаться так, что парсер встретил эту строку, а на самом деле это начало
// диагностики.
// Если есть путь подключения файла, то сразу не подходит: в этой строке первые несколько
// символов образуют "In file included".
// Если пути подключения файла нет: если дальше идёт путь к файлу, то либо первые несколько
// символов пути образуют строку "cc1plus:", что нарушает условие, что в файле не встречается
// двоеточий (в таком случае мы перестанем читать диагностики, дальше будем ожидать переводы строки,
// если будут одни лишь переводы строки, значит перед нами была не диагностика, но раньше всё
// синтаксису соответствовало, потому соответствует синстаксису вывода, мы успешно разберём),
// либо путь к файлу является строкой, которую образуют первые несколько символов "cc1plus",
// тогда, если ...
// TODO: дописать! Рассматривать случай невалидного файла?
const char Parser::WERROR_PREFIX[] = "cc1plus: all warnings being treated as errors";
const std::size_t Parser::WERROR_PREFIX_LEN = sizeof(WERROR_PREFIX) / sizeof(char) - 1;

// Имя файла не должно содержать в себе эту строку.
// TODO: написать рассуждение, подобное тому, которое написано про WERROR_STRING?
const char Parser::INCLUSION_PATH_PREFIX[] = "In file included from ";
const std::size_t Parser::INCLUSION_PATH_PREFIX_LEN =
	sizeof(INCLUSION_PATH_PREFIX) / sizeof(char) - 1;

const char Parser::IN_FUNCTION_PREFIX[] = " In function '";
const std::size_t Parser::IN_FUNCTION_PREFIX_LEN = sizeof(IN_FUNCTION_PREFIX) / sizeof(char) - 1;

const char Parser::IN_FUNCTION_ENDING[] = "':\n";
const std::size_t Parser::IN_FUNCTION_ENDING_LEN = sizeof(IN_FUNCTION_ENDING) / sizeof(char) - 1;

const char Parser::ERROR_PREFIX[] = ": error: ";
const std::size_t Parser::ERROR_PREFIX_LEN = sizeof(ERROR_PREFIX) / sizeof(char) - 1;

const char Parser::WARNING_PREFIX[] = ": warning: ";
const std::size_t Parser::WARNING_PREFIX_LEN = sizeof(WARNING_PREFIX) / sizeof(char) - 1;

const char Parser::NOTE_PREFIX[] = ": note: ";
const std::size_t Parser::NOTE_PREFIX_LEN = sizeof(NOTE_PREFIX) / sizeof(char) - 1;

const uint32_t Parser::MAX_NUMBER_LEN = 8;

const char Parser::INCLUSION_PATH_CONTINUATION_PREFIX[] = ",\n                 from ";
const std::size_t Parser::INCLUSION_PATH_CONTINUATION_PREFIX_LEN =
	sizeof(INCLUSION_PATH_CONTINUATION_PREFIX) / sizeof(char) - 1;

const char Parser::INCLUSION_PATH_INSTANTIATION_PREFIX[] = " In instantiation of ";
const std::size_t Parser::INCLUSION_PATH_INSTANTIATION_PREFIX_LEN =
	sizeof(INCLUSION_PATH_INSTANTIATION_PREFIX) / sizeof(char) - 1;

// На следующей итерации надо в обоих случаях учитывать пробел в конце.
const char Parser::INCLUSION_PATH_SUBSTITUTION_PREFIX[] = " In substitution of";
const std::size_t Parser::INCLUSION_PATH_SUBSTITUTION_PREFIX_LEN =
	sizeof(INCLUSION_PATH_SUBSTITUTION_PREFIX) / sizeof(char) - 1;

const char Parser::INCLUSION_PATH_REQUIRED_FROM_PREFIX[] = ":   required from ";
const std::size_t Parser::INCLUSION_PATH_REQUIRED_FROM_PREFIX_LEN =
	sizeof(INCLUSION_PATH_REQUIRED_FROM_PREFIX) / sizeof(char) - 1;

const char Parser::INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX[] = ":   required from here\n";
const std::size_t Parser::INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX_LEN =
	sizeof(INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX) / sizeof(char) - 1;

const char Parser::CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX[] = " |";
const std::size_t Parser::CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX_LEN =
	sizeof(CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX) / sizeof(char) - 1;

const char Parser::REFERENCE_PREFIX[] = "      |";
const std::size_t Parser::REFERENCE_PREFIX_LEN = sizeof(REFERENCE_PREFIX) / sizeof(char) - 1;

// TODO: придумать другие сообщения об ошибках? Решить, что они должны сообщать?

// Путь к файлу находится в началах обоих возможных частей в выражении, начинающемся на
// второй строке синтаксиса диагностики, потому не удаётся по текущему символу и фиксированному
// количеству последующих определить, какую из частей дальше ожидать.
// TODO: учитывать это в синтаксисе второй итерации.
// Сейчас кажется, что стоит замечания относить к последней ошибке или предупреждению, но будем
// поступать так, как договорились в "syntax.txt". Следующие итерации покажут, нужно ли так делать.
// Поскольку мы сразу добавляем замечание к последней ошибке, вложенную диагностику будем добавлять
// туда же в замечания.
std::vector<Diagnostic> Parser::readOutput() {
	std::vector<Diagnostic> diagnostics;
	bool nextIsSubDiag = false;
	while (buffer[0] != END_OF_FILE && !isPrefixOfBuffer(WERROR_PREFIX) && buffer[0] != '\n') {
		Diagnostic diagnostic;
		if (isPrefixOfBuffer(INCLUSION_PATH_PREFIX)) {
			diagnostic.detailedDescription += readInclusionPath();
		}
		diagnostic.detailedDescription += readFilePath();
		diagnostic.detailedDescription += readColon();
		if (isPrefixOfBuffer(IN_FUNCTION_PREFIX)) {
			diagnostic.detailedDescription += IN_FUNCTION_PREFIX;
			read(IN_FUNCTION_PREFIX_LEN);
			diagnostic.detailedDescription += readFunctionSignature();
			check(
				isPrefixOfBuffer(IN_FUNCTION_ENDING),
				"Expected ending of the \" In function\" block, but it is not present."
			);
			diagnostic.detailedDescription += IN_FUNCTION_ENDING;
			read(IN_FUNCTION_ENDING_LEN);

			diagnostic.detailedDescription += readFilePath();
			diagnostic.detailedDescription += readColon();
			diagnostic.lineNum = readLineNumber();
			diagnostic.detailedDescription += std::to_string(diagnostic.lineNum);
			diagnostic.detailedDescription += readColon();
			diagnostic.detailedDescription += std::to_string(readColumnNumber());

			check(
				isPrefixOfBuffer(ERROR_PREFIX) ||
				isPrefixOfBuffer(WARNING_PREFIX) ||
				isPrefixOfBuffer(NOTE_PREFIX),
				"Expected diagnostics type (error, warning or note), but it is not present."
			);
			if (isPrefixOfBuffer(ERROR_PREFIX)) {
				diagnostic.diagType = DiagnosticType::ERROR;
				diagnostic.detailedDescription += ERROR_PREFIX;
				read(ERROR_PREFIX_LEN);
			} else if (isPrefixOfBuffer(WARNING_PREFIX)) {
				diagnostic.diagType = DiagnosticType::WARNING;
				diagnostic.detailedDescription += WARNING_PREFIX;
				read(WARNING_PREFIX_LEN);
			} else if (isPrefixOfBuffer(NOTE_PREFIX)) {
				diagnostic.diagType = DiagnosticType::NOTE;
				diagnostic.detailedDescription += NOTE_PREFIX;
				read(NOTE_PREFIX_LEN);
			} else throw;
		} else if (
			isPrefixOfBuffer(INCLUSION_PATH_INSTANTIATION_PREFIX) ||
			isPrefixOfBuffer(INCLUSION_PATH_SUBSTITUTION_PREFIX)
		) {
			diagnostic.detailedDescription += readTemplateExpansionPath(diagnostic.lineNum);

			diagnostic.detailedDescription += readFilePath();
			diagnostic.detailedDescription += readColon();
			diagnostic.detailedDescription += std::to_string(readLineNumber());
			diagnostic.detailedDescription += readColon();
			diagnostic.detailedDescription += std::to_string(readColumnNumber());

			check(
				isPrefixOfBuffer(ERROR_PREFIX) ||
				isPrefixOfBuffer(WARNING_PREFIX) ||
				isPrefixOfBuffer(NOTE_PREFIX),
				"Expected diagnostics type (error, warning or note), but it is not present."
			);
			if (isPrefixOfBuffer(ERROR_PREFIX)) {
				diagnostic.diagType = DiagnosticType::ERROR;
				diagnostic.detailedDescription += ERROR_PREFIX;
				read(ERROR_PREFIX_LEN);
			} else if (isPrefixOfBuffer(WARNING_PREFIX)) {
				diagnostic.diagType = DiagnosticType::WARNING;
				diagnostic.detailedDescription += WARNING_PREFIX;
				read(WARNING_PREFIX_LEN);
			} else if (isPrefixOfBuffer(NOTE_PREFIX)) {
				diagnostic.diagType = DiagnosticType::NOTE;
				diagnostic.detailedDescription += NOTE_PREFIX;
				read(NOTE_PREFIX_LEN);
			} else throw;
		} else {
			diagnostic.lineNum = readLineNumber();
			diagnostic.detailedDescription += std::to_string(diagnostic.lineNum);
			diagnostic.detailedDescription += readColon();
			diagnostic.detailedDescription += std::to_string(readColumnNumber());

			check(
				isPrefixOfBuffer(ERROR_PREFIX) ||
				isPrefixOfBuffer(WARNING_PREFIX) ||
				isPrefixOfBuffer(NOTE_PREFIX),
				"Expected diagnostics type (error, warning or note), but it is not present."
			);
			if (isPrefixOfBuffer(ERROR_PREFIX)) {
				diagnostic.diagType = DiagnosticType::ERROR;
				diagnostic.detailedDescription += ERROR_PREFIX;
				read(ERROR_PREFIX_LEN);
			} else if (isPrefixOfBuffer(WARNING_PREFIX)) {
				diagnostic.diagType = DiagnosticType::WARNING;
				diagnostic.detailedDescription += WARNING_PREFIX;
				read(WARNING_PREFIX_LEN);
			} else if (isPrefixOfBuffer(NOTE_PREFIX)) {
				diagnostic.diagType = DiagnosticType::NOTE;
				diagnostic.detailedDescription += NOTE_PREFIX;
				read(NOTE_PREFIX_LEN);
			} else throw;
		}

		bool isSubDiag = nextIsSubDiag;
		nextIsSubDiag = false;
		if (isPrefixOfBuffer("  ")) {
			diagnostic.description = readDescriptionWithTwoSpaces();
			isSubDiag = true;
		} else {
			diagnostic.description = readDescription();
		}
		diagnostic.detailedDescription += diagnostic.description;
		diagnostic.detailedDescription += readLinefeed();
		if (isPrefixOfBuffer(" ")) {
			diagnostic.detailedDescription += readCodeExcerptWithReferences();
		}
		check(
			!diagnostic.description.empty(),
			"Expected description of the diagnostic, but it is not present."
		);
		if (diagnostic.description.back() == ':') {
			nextIsSubDiag = true;
		}

		if (diagnostic.diagType == DiagnosticType::NOTE || isSubDiag) {
			check(
				!diagnostics.empty(),
				"Expected a diagnostics before current diagnostic to add notes and subdiagnostics to, "
				"but it is not present."
			);
			diagnostics.back().notes += diagnostic.detailedDescription;
		} else {
			diagnostics.push_back(std::move(diagnostic));
		}
	}

	if (isPrefixOfBuffer(WERROR_PREFIX)) {
		read(WERROR_PREFIX_LEN);
	}

	while (buffer[0] == '\n') {
		read(1);
	}

	check(buffer[0] == END_OF_FILE, "Done processing the input file, but it has not ended.");

	return diagnostics;
}

std::string Parser::readInclusionPath() {
	check(
		isPrefixOfBuffer(INCLUSION_PATH_PREFIX),
		"Expected inclusion path prefix, but it is not present."
	);
	std::string result(INCLUSION_PATH_PREFIX);
	read(INCLUSION_PATH_PREFIX_LEN);

	result += readFilePath();
	result += readColon();
	result += std::to_string(readLineNumber());

	while (isPrefixOfBuffer(INCLUSION_PATH_CONTINUATION_PREFIX)) {
		result += INCLUSION_PATH_CONTINUATION_PREFIX;
		read(INCLUSION_PATH_CONTINUATION_PREFIX_LEN);
		result += readFilePath();
		result += readColon();
		result += std::to_string(readLineNumber());
	}
	result += readColon();
	result += readLinefeed();
	return result;
}

bool Parser::isFilePathChar() {
	return buffer[0] != END_OF_FILE && buffer[0] != ':' && buffer[0] != '\n';
}

// Временное решение, позже будем читать строку так, чтобы она не содержала
// "In file included from ".
std::string Parser::readFilePath() {
	check(isFilePathChar(), "Expected a file path's char.");
	std::string result(buffer, 1);
	read(1);
	while (isFilePathChar()) {
		result += buffer[0];
		read(1);
	}
	return result;
}

bool Parser::isFunctionSignatureChar() {
	return buffer[0] != END_OF_FILE && buffer[0] != ':' && buffer[0] != '\n' && buffer[0] != '\'';
}

std::string Parser::readFunctionSignature() {
	check(isFunctionSignatureChar(), "Expected a function signature's char.");
	std::string result(buffer, 1);
	read(1);
	while (isFunctionSignatureChar()) {
		result += buffer[0];
		read(1);
	}
	return result;
}

// TODO: обратить внимание на синтаксис пути раскрытия выражения в шаболне.
// TODO: дописать сообщение для check.
std::string Parser::readTemplateExpansionPath(uint32_t& lineNum) {
	std::string result;

	check(
		isPrefixOfBuffer(INCLUSION_PATH_INSTANTIATION_PREFIX) ||
		isPrefixOfBuffer(INCLUSION_PATH_SUBSTITUTION_PREFIX),
		"Expected \"In instantiation of \" or \"In substitution of\", but both of them are not present."
	);
	if (isPrefixOfBuffer(INCLUSION_PATH_INSTANTIATION_PREFIX)) {
		result += INCLUSION_PATH_INSTANTIATION_PREFIX;
		read(INCLUSION_PATH_INSTANTIATION_PREFIX_LEN);
	} else if (isPrefixOfBuffer(INCLUSION_PATH_SUBSTITUTION_PREFIX)) {
		result += INCLUSION_PATH_SUBSTITUTION_PREFIX;
		read(INCLUSION_PATH_SUBSTITUTION_PREFIX_LEN);
	} else throw;
	result += readString();
	result += readLinefeed();
	while (buffer[0] != END_OF_FILE) {
		result += readFilePath();
		result += readColon();
		lineNum = readLineNumber();
		result += std::to_string(lineNum);
		result += readColon();
		result += std::to_string(readColumnNumber());
		check(
			isPrefixOfBuffer(INCLUSION_PATH_REQUIRED_FROM_PREFIX) ||
			isPrefixOfBuffer(INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX),
			"Exxpected "
		);
		if (isPrefixOfBuffer(INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX)) {
			result += INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX;
			read(INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX_LEN);
			break;
		} else if (isPrefixOfBuffer(INCLUSION_PATH_REQUIRED_FROM_PREFIX)) {
			result += INCLUSION_PATH_REQUIRED_FROM_PREFIX;
			read(INCLUSION_PATH_REQUIRED_FROM_PREFIX_LEN);
			result += readString();
			result += readLinefeed();
		} else throw;
	}
	return result;
}

std::string Parser::readColon() {
	check(isPrefixOfBuffer(":"), "Expected a colon, but it is not present.");
	read(1);
	return std::string(":");
}

std::string Parser::readLinefeed() {
	check(isPrefixOfBuffer("\n"), "Expected a linefeed, but it is not present.");
	read(1);
	return std::string("\n");
}

std::string Parser::readSpace() {
	check(isPrefixOfBuffer(" "), "Expected a space, but it is not present.");
	read(1);
	return std::string(" ");
}

uint32_t Parser::readLineNumber() {
	return readNumber();
}

uint32_t Parser::readColumnNumber() {
	return readNumber();
}

// В синтаксисе не огововаривали, но ограничим длину числа 8-ю символами.
uint32_t Parser::readNumber() {
	if (buffer[0] == '0') {
		read(1);
		return 0;
	}
	check(
		'1' <= buffer[0] && buffer[0] <= '9',
		"Failed to read the number: expected a character in range from 1 to 9, but current character "
		"is not in that range."
	);
	uint32_t result = static_cast<uint32_t>(buffer[0] - '0');
	read(1);
	for (uint32_t numberLen = 1; numberLen < MAX_NUMBER_LEN; ++numberLen) {
		if (buffer[0] < '0' || buffer[0] > '9') {
			break;
		}
		result *= 10;
		result += static_cast<uint32_t>(buffer[0] - '0');
		read(1);
	}
	return result;
}

std::string Parser::readDescription() {
	// TODO: другое сообщение?
	std::string result;
	for (int i = 0; i < 2; ++i) {
		check(buffer[0] != END_OF_FILE && buffer[0] != ' ', "Expected a non-space character.");
		result += buffer[0];
		read(1);
	}
	result += readString();
	return result;
}

std::string Parser::readDescriptionWithTwoSpaces() {
	// TODO: другое сообщение?
	std::string result;
	for (int i = 0; i < 2; ++i) {
		check(buffer[0] != END_OF_FILE && buffer[0] == ' ', "Expected space character.");
		result += buffer[0];
		read(1);
	}
	result += readString();
	return result;
}

bool Parser::isStringChar() {
	return buffer[0] != END_OF_FILE && buffer[0] != '\n';
}

std::string Parser::readString() {
	check(isStringChar(), "Expected a string char.");
	std::string result(buffer, 1);
	read(1);
	while (isStringChar()) {
		result += buffer[0];
		read(1);
	}
	return result;
}

// TODO: дописать сообщение для check.
std::string Parser::readReference() {
	check(isPrefixOfBuffer(REFERENCE_PREFIX), "Expected ");
	std::string result;
	result += REFERENCE_PREFIX;
	read(REFERENCE_PREFIX_LEN);

	result += readString();
	result += readLinefeed();
	return result;
}

std::string Parser::readReferences() {
	std::string result;
	result += readReference();
	while (isPrefixOfBuffer(REFERENCE_PREFIX)) {
		result += readReference();
	}
	return result;
}

// TODO: дописать сообщение для check.
// Возможно, (очень вероятно) что слово "references" не подходит в качестве перевода.
std::string Parser::readCodeExcerptWithReferences() {
	std::string result;
	result += readSpace();
	while (buffer[0] == ' ') {
		result += buffer[0];
		read(1);
	}
	result += std::to_string(readLineNumber());
	check(isPrefixOfBuffer(CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX), "Expected ");
	result += CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX;
	read(CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX_LEN);
	result += readString();
	result += readLinefeed();
	result += readReferences();
	return result;
}

void Parser::read(std::size_t nToRead) {
	if (inputStream.eof()) {
		std::size_t eofPos = 0;
		for (eofPos = 0; eofPos < BUFFER_SIZE; ++eofPos) {
			if (buffer[eofPos] == END_OF_FILE) {
				break;
			}
		}
		assert(eofPos < BUFFER_SIZE);
		nToRead = std::min(nToRead, eofPos);
	}
	if (nToRead == 0) {
		return;
	}
	assert(nToRead <= BUFFER_SIZE);
	for (std::size_t pos = 0; pos < BUFFER_SIZE - nToRead; ++pos) {
		buffer[pos] = buffer[pos + nToRead];
	}
	// TODO: Узнать, можно ли читать, если поток уже закончился?
	inputStream.read(buffer + BUFFER_SIZE - nToRead, nToRead);
	if (inputStream.bad()) {
		throw std::runtime_error("Failed to read from the input stream!");
	} else if (inputStream.eof()) {
		std::streamsize nRead = inputStream.gcount();
		assert(nRead >= 0);
		assert(static_cast<std::size_t>(nRead) < nToRead);
		buffer[BUFFER_SIZE - nToRead + nRead] = END_OF_FILE;
	}
}

void Parser::check(bool condition, const char* message) {
	if (!condition) {
		std::cout << buffer << '\n';
		throw std::runtime_error(message);
	}
}

bool Parser::isPrefixOfBuffer(const char* str) {
	for (std::size_t pos = 0; pos < BUFFER_SIZE && *str != '\0'; ++pos, ++str) {
		if (buffer[pos] != *str) {
			return false;
		}
	}
	return *str == '\0';
}
