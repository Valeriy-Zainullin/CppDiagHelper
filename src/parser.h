#include <cstdint>
#include <vector>
#include <string>
#include <istream>


namespace diags {
	namespace parsing {
		enum class DiagnosticType { ERROR, WARNING, NOTE };
		struct Diagnostic {
			uint32_t lineNum;
			DiagnosticType diagType;
			std::string description;

			std::string detailedDescription;
			std::string notes;
		};

		// TODO: Отказываться разбирать файлы слишком большого размера! Возможно, делать это не здесь.
		// Разбирает вывод компилятора, вызванного с локалью "C".
		class Parser {
		public:
			Parser(std::istream& inputStreamParam);

			Parser& operator=(const Parser& other) = delete;

			std::vector<Diagnostic> parse();
		private:
			// Не можем определить в файле с исходным кодом, поскольку нужно для определения структуры
			// в заголовочном файле. Потому определяем здесь.
			static const std::size_t BUFFER_SIZE = 64;

			// Этот символ никогда не должен встречаться во входном файле.
			static const char END_OF_FILE;

			static const char WERROR_PREFIX[];
			static const std::size_t WERROR_PREFIX_LEN;

			static const char INCLUSION_PATH_PREFIX[];
			static const std::size_t INCLUSION_PATH_PREFIX_LEN;

			static const char IN_FUNCTION_PREFIX[];
			static const std::size_t IN_FUNCTION_PREFIX_LEN;

			static const char IN_FUNCTION_ENDING[];
			static const std::size_t IN_FUNCTION_ENDING_LEN;

			static const char ERROR_PREFIX[];
			static const std::size_t ERROR_PREFIX_LEN;

			static const char WARNING_PREFIX[];
			static const std::size_t WARNING_PREFIX_LEN;

			static const char NOTE_PREFIX[];
			static const std::size_t NOTE_PREFIX_LEN;

			static const uint32_t MAX_NUMBER_LEN;

			static const char INCLUSION_PATH_CONTINUATION_PREFIX[];
			static const std::size_t INCLUSION_PATH_CONTINUATION_PREFIX_LEN;

			static const char INCLUSION_PATH_INSTANTIATION_PREFIX[];
			static const std::size_t INCLUSION_PATH_INSTANTIATION_PREFIX_LEN;

			static const char INCLUSION_PATH_SUBSTITUTION_PREFIX[];
			static const std::size_t INCLUSION_PATH_SUBSTITUTION_PREFIX_LEN;

			static const char INCLUSION_PATH_REQUIRED_FROM_PREFIX[];
			static const std::size_t INCLUSION_PATH_REQUIRED_FROM_PREFIX_LEN;

			static const char INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX[];
			static const std::size_t INCLUSION_PATH_REQUIRED_FROM_HERE_PREFIX_LEN;

			static const char CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX[];
			static const std::size_t CODE_EXCERPT_WITH_REFERENCES_SEP_PREFIX_LEN;

			static const char REFERENCE_PREFIX[];
			static const std::size_t REFERENCE_PREFIX_LEN;
		private:
			std::vector<Diagnostic> readOutput();

			std::string readInclusionPath();

			bool isFilePathChar();
			std::string readFilePath();

			bool isFunctionSignatureChar();
			std::string readFunctionSignature();

			std::string readTemplateExpansionPath();

			std::string readColon();
			std::string readLinefeed();

			uint32_t readLineNumber();
			uint32_t readColumnNumber();
			uint32_t readNumber();

			std::string readDescription();

			bool isStringChar();
			std::string readString();

			std::string readReference();
			std::string readReferences();
			std::string readCodeExcerptWithReferences();

			void read(std::size_t nToRead);
			void check(bool condition, const char* message);
			bool isPrefixOfBuffer(const char* str);
		private:
			char buffer[BUFFER_SIZE];
			std::istream& inputStream;
		};
	}
}
