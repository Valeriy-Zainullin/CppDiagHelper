#ifndef UI_H
#define UI_H

#include <cstddef>

namespace diags {
	namespace ui {
		class Console {
		public:
			Console();
			Console(const Console& other) = delete;
			~Console();

			Console& operator=(const Console& other) = delete;

			void getSize(unsigned int& width, unsigned int& height);
			void write(const char* str);
		};

		class InterfaceObject {
		protected:
			InterfaceObject(Console& consoleParam);
			InterfaceObject(const InterfaceObject& other) = delete;

			InterfaceObject& operator=(const InterfaceObject& other) = delete;

			void drawTopBorder(unsigned int width);
			void drawEmptyLine(unsigned int width);
			void drawSeparationLine(unsigned int width);
			void drawBottomBorder(unsigned int width);
		protected:
			Console& console;
		private:
			static const unsigned int PATTERN_MIN_LEN;
		private:
			void drawPattern(
				const char* firstChar,
				const char* fillChar,
				const char* lastChar,
				unsigned int width
			);
		};

		class DiagnosticsList : public InterfaceObject {
		public:
			DiagnosticsList(Console& console);

			void draw(unsigned int width, unsigned int height);
		private:
			static const char HEADING_CONST_PART[];
			static const std::size_t HEADING_CONST_PART_LEN;
			static const unsigned int LINE_COL_WIDTH;
			static const unsigned int TYPE_COL_WIDTH;
			static const unsigned int DELIMETERS_BEFORE_CONTENT_COL;
			static const unsigned int CONTENT_COL_CONST_PART_LEN;
			static const unsigned int SEPARATOR_MIN_LEN;
		private:
			unsigned int getLineColWidth(unsigned int totalWidth);
			unsigned int getTypeColWidth(unsigned int totalWidth);
			unsigned int getContentColWidth(unsigned int totalWidth);

			void drawHeading(unsigned int width);
			void drawItems(unsigned int width, unsigned int height);
		};

		class NavigationBar : public InterfaceObject {
		public:
			NavigationBar(Console& consoleParam);

			void draw(unsigned int width);
		};

		class DetailedView : public InterfaceObject {
		public:
			DetailedView(Console& consoleParam);

			void draw(unsigned int width, unsigned int height);
		};

		class InformationPanel : public InterfaceObject {
		public:
			InformationPanel(Console& consoleParam);

			void draw(unsigned int width);
		};

		class Interface : public InterfaceObject {
		public:
			Interface(Console& consoleParam);

			void runEventLoop();
		private:
			unsigned int getDiagListHeight(unsigned int totalHeight);
			unsigned int getDetailedViewHeight(unsigned int totalHeight);

			void draw();
		private:
			DiagnosticsList diagList;
			NavigationBar navBar;
			DetailedView detailedView;
			InformationPanel infoPanel;
		};
	}
}

#endif
