#include <cstdint>
#include <vector>
#include <string>


namespace diags {
	namespace parsing {
		enum class DiagnosticType { ERROR, WARNING };
		struct Diagnostic {
			uint64_t lineNum;
			DiagnosticType diagType;
			std::string description;

			std::string detailedDescription;
			std::string notes;
			std::string hints;
			std::string solutions;
		};

		std::vector<Diagnostic> parseFile(const char* fileName);
	}
}
