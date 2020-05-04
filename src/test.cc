#include "parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <filesystem>

namespace diags::testing::parsing {
	std::string readFile(std::istream& inputStream) {
		std::string contents{std::istreambuf_iterator<char>(inputStream), {}};
		return contents;
	}

	void dumpDiagnostic(std::ostream& outputStream, const diags::parsing::Diagnostic& diagnostic) {
		outputStream << '\n';
		outputStream << diagnostic.lineNum << '\n';

		if (diagnostic.diagType == diags::parsing::DiagnosticType::ERROR) {
			outputStream << "ERROR" << '\n';
		} else if (diagnostic.diagType == diags::parsing::DiagnosticType::WARNING) {
			outputStream << "WARNING" << '\n';
		} else throw;

		outputStream << diagnostic.detailedDescription << '\n';
		outputStream << diagnostic.notes << '\n';
	}

	std::string dumpDiagnostics(const std::vector<diags::parsing::Diagnostic>& diagnostics) {
		std::stringstream outputStream;
		outputStream.exceptions(std::ios_base::failbit | std::ios_base::badbit);

		outputStream << diagnostics.size() << '\n';
		for (const diags::parsing::Diagnostic& diagnostic: diagnostics) {
			dumpDiagnostic(outputStream, diagnostic);
		}

		std::string result = outputStream.str();
		return result;
	}

	std::string adjacentFilePath(const std::string& basePath, const std::string& fileName) {
		std::string basePathCopy = basePath;
		std::filesystem::path filePath(basePathCopy);
		filePath.replace_filename(fileName);
		return filePath.u8string();
	}

	bool readLineFeed(std::istream& inputStream) {
		char lineFeed;
		inputStream.get(lineFeed);
		if (!inputStream.bad() && !inputStream.eof() && lineFeed != '\n') {
			std::cout << "Файл со списком тестов не соответствует синтаксису: ";
			std::cout << "ожидался перевод строки, но он не был встречен." << '\n';
			return false;
		}
		return true;
	}

	bool testParser(const std::string& testSetName, const std::string& testListPath) {
		std::cout << testSetName << '.' << '\n';

		std::ifstream testListFileStream(testListPath);
		if (testListFileStream.fail()) {
			std::cout << "Не удалось открыть файл со списком тестов." << '\n';
			return false;
		}

		int nTests;
		std::string nTestsAsStr;
		std::getline(testListFileStream, nTestsAsStr);
		if (testListFileStream.eof() || testListFileStream.fail()) {
			std::cout << "Не удалось прочитать количество тестов." << '\n';
			return false;
		}
		nTests = std::stoi(nTestsAsStr);

		bool hasPassed = true;
		for (int i = 0; i < nTests; ++i) {
			std::string testName;
			std::string testFileName;
			std::string testAnswerFileName;

			if (!readLineFeed(testListFileStream)) {
				return false;
			}
			std::getline(testListFileStream, testName);
			std::getline(testListFileStream, testFileName);
			std::getline(testListFileStream, testAnswerFileName);

			std::string testFilePath = adjacentFilePath(testListPath, testFileName);
			std::string testAnswerFilePath = adjacentFilePath(testListPath, testAnswerFileName);

			if (testListFileStream.bad()) {
				std::cout << "Ошибка при чтении файла со списком тестов." << '\n';
				return false;
			} else if (testListFileStream.eof()) {
				std::cout << "Неожиданный конец файла со списком тестов." << '\n';
				return false;
			} else if (testListFileStream.fail()) {
				std::cout << "Файл со списком тестов не соответствует синтаксису." << '\n';
				return false;
			}

			std::ifstream testFileStream(testFilePath);
			if (testFileStream.fail()) {
				std::cout << "Не удалось открыть файл с тестом. Путь: \"" << testFilePath << "\"." << '\n';
				return false;
			}

			std::ifstream testAnswerFileStream(testAnswerFilePath);
			if (testAnswerFileStream.fail()) {
				std::cout << "Не удалось открыть файл с ответом на тест. Путь: \"" << testAnswerFilePath;
				std::cout << "\"." << '\n';
				return false;
			}

			std::string testAnswer = readFile(testAnswerFileStream);
			if (testAnswerFileStream.fail()) {
				std::cout << "Не удалось прочитать ответ на тест." << '\n';
				return false;
			}

			try {
				std::vector<diags::parsing::Diagnostic> diagnostics =
					diags::parsing::Parser(testFileStream).parse();
				if (dumpDiagnostics(diagnostics) != testAnswer) {
					std::cout << testName << ": ответ неверный (дампы не совпали)." << '\n';
					hasPassed = false;
				} else {
					std::cout << testName << ": OK." << '\n';
				}
			} catch (std::exception& exc) {
				std::cout << testName << ": парсер бросил исключение. Описание: \"" << exc.what();
				std::cout << "\"." << '\n';
				hasPassed = false;
			}
		}
		return hasPassed;
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Использование: \"" << argv[0] << " [название набора] [путь к списку]\"." << '\n';
		std::cout << "Проверяет парсер на тестах из набора в порядке следования в списке." << '\n';
		std::cout << "Внимание: программа не проверяет размер входных файлов, но ей приходится ";
		std::cout << "читать полностью их содержимое в память." << '\n';
		return 1;
	}

	std::string testSetName = argv[1];
	std::string testListPath = argv[2];
	if (!diags::testing::parsing::testParser(testSetName, testListPath)) {
		return 2;
	}

	return 0;
}
