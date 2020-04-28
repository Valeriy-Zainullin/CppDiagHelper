#include <cstdint>
#include <vector>
#include <string>
#include <istream>


namespace diags {
	namespace parsing {
		enum class DiagnosticType { ERROR, WARNING };
		struct Diagnostic {
			uint32_t lineNum;
			DiagnosticType diagType;
			std::string description;

			std::string detailedDescription;
			std::string notes;
			std::string hints;
			std::string solutions;
		};

		// TODO: Отказываться разбирать файлы слишком большого размера! Возможно, делать это не здесь.
		// Разбирает вывод компилятора, вызванного с локалью "C".
		class Parser {
		public:
			Parser(std::istream& inputStreamParam);

			Parser& operator=(const Parser& other) = delete;

			std::vector<Diagnostic> parse();
		private:
			static const std::size_t BUFFER_SIZE;
			static const char END_OF_FILE;
		private:
			std::vector<Diagnostic> readOutput();
			std::string readInclusionPath();
			std::string readFilePath();
			std::string readFunctionSignature();
			std::string readTemplateExpansionPath();
			uint32_t readLineNumber();
			uint32_t readColumnNumber();
			uint32_t readNumber();
			std::string readDescription();
			std::string readString();
			std::string readConcretization();

			void read();
			void check(bool condition);
		private:
			char buffer[BUFFER_SIZE];
			std::istream inputStream;
		};
	}
}
