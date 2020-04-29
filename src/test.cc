#include "parser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <exception>
#include <cstring>
#include <algorithm>

namespace diags::testing::parsing {
	struct Test {
		std::string testFilePath;
		std::vector<diags::parsing::Diagnostic> diagnostics;
	};

	void readLineFeed(std::istream& inputStream) {
		char lineFeed;
		inputStream.read(&lineFeed, 1);
		if (lineFeed != '\n') {
			throw std::runtime_error("Expected a line feed, but it is not present.");
		}
	}

	std::string readBlock(std::istream& inputStream, char* buffer, std::size_t bufferSize) {
		const std::size_t maxNumOfLinesInBlock = 200;
		std::string result;
		for (std::size_t i = 0; i < maxNumOfLinesInBlock; ++i) {
			// Проверять, что буффера хватило, захватили перевод строки.
			inputStream.getline(buffer, bufferSize);
			if (buffer[0] == '\n') {
				break;
			}
			result += buffer;
		}
		if (buffer[0] != '\n') {
			throw std::runtime_error("Too many lines in a block.");
		}
		return result;
	}

	bool isEqual(
		const std::vector<diags::parsing::Diagnostic>& lhs,
		const std::vector<diags::parsing::Diagnostic>& rhs
	) {
		if (lhs.size() != rhs.size()) {
			return false;
		}
		for (std::size_t pos = 0; pos < lhs.size(); ++pos) {
			if (!(
				lhs[pos].lineNum == rhs[pos].lineNum &&
				lhs[pos].diagType == rhs[pos].diagType &&
				lhs[pos].description == rhs[pos].description &&
				lhs[pos].detailedDescription == rhs[pos].detailedDescription &&
				lhs[pos].notes == rhs[pos].notes
			)) {
				return false;
			}
		}
		return true;
	}

	std::vector<diags::parsing::Diagnostic> decodeTestAnswer(std::string& testAnswerPath) {
		std::ifstream inputStream(testAnswerPath);
		inputStream.exceptions(std::ios_base::failbit | std::ios_base::badbit);

		const std::size_t bufferSize = 1024;
		char buffer[bufferSize];

		unsigned int nDiags;
		inputStream >> nDiags;
		readLineFeed(inputStream);

		std::vector<diags::parsing::Diagnostic> diagnostics;

		for (unsigned int i = 0; i < nDiags; ++i) {
			diags::parsing::Diagnostic diagnostic;

			readLineFeed(inputStream);

			inputStream >> diagnostic.lineNum;
			readLineFeed(inputStream);

			inputStream.getline(buffer, bufferSize);
			if (std::strcmp(buffer, "ERROR\n") == 0) {
				diagnostic.diagType = diags::parsing::DiagnosticType::ERROR;
			} else if (std::strcmp(buffer, "WARNING\n") == 0) {
				diagnostic.diagType = diags::parsing::DiagnosticType::WARNING;
			} else {
				throw std::runtime_error("Unsupported diagnostic type.");
			}
			readLineFeed(inputStream);

			diagnostic.description = readBlock(inputStream, buffer, bufferSize);
			// Проверять, что это всегда так?
			if (diagnostic.description.back() == '\n') {
				diagnostic.description.pop_back();
			}

			diagnostic.detailedDescription = readBlock(inputStream, buffer, bufferSize);
			diagnostic.notes = readBlock(inputStream, buffer, bufferSize);

			diagnostics.push_back(std::move(diagnostic));
		}

		return diagnostics;
	}

	bool testParser(std::string testDirPath) {
		std::cout << testDirPath << ':' << std::endl;

		const std::string testAnswerFileNameSuffix = "_expected.txt";
		std::vector<std::filesystem::path> testAnswerPaths;
		for (const auto& entry: std::filesystem::directory_iterator(testDirPath)) {
			if (entry.is_regular_file()) {
				std::string fileName = entry.path().filename().generic_u8string();
				std::string::size_type pos = fileName.rfind(testAnswerFileNameSuffix);
				if (pos != std::string::npos && fileName.size() - pos == testAnswerFileNameSuffix.size()) {
					testAnswerPaths.push_back(entry);
				}
			}
		}
		std::sort(testAnswerPaths.begin(), testAnswerPaths.end());

		bool passed = true;
		const std::string testFileNameSuffix = ".txt";
		for (const auto& testAnswerPath: testAnswerPaths) {
			std::string testAnswerFileName = testAnswerPath.filename().generic_u8string();
			std::string testName = testAnswerFileName.substr(
				0,
				testAnswerFileName.size() - testAnswerFileNameSuffix.size()
			);
			std::string testFileName = testName + testFileNameSuffix;
			std::filesystem::path testPath = testAnswerPath;
			testPath.replace_filename(testFileName);
			if (std::filesystem::exists(testPath)) {
				std::ifstream inputStream(testPath.relative_path().generic_u8string());
				std::string testAnswerPathString = testAnswerPath.relative_path().generic_u8string();
				std::vector<diags::parsing::Diagnostic> answers = decodeTestAnswer(testAnswerPathString);
				try {
					if (isEqual(diags::parsing::Parser(inputStream).parse(), answers)) {
						std::cout << testName << ": OK." << std::endl;
					} else {
						std::cout << testName << ": FAILED (wrong answer)." << std::endl;
						passed = false;
					}
				} catch (std::exception& exc) {
					std::cout << testName << ": FAILED (exception has been thrown).\n";
					std::cout << exc.what() << std::endl;
				}
			} else {
				std::cout << testName << ": test file is not found!" << std::endl;
			}
		}
		return passed;
	}
}

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		diags::testing::parsing::testParser(std::string(argv[i]));
	}
	return 0;
}
