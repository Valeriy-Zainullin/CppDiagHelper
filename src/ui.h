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
		};

		class DiagnosticsList {
		public:
			DiagnosticsList() = default;
			DiagnosticsList(const DiagnosticsList& other) = delete;
			~DiagnosticsList() = default;

			DiagnosticsList& operator=(const DiagnosticsList& other) = delete;

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
			void drawSeparator(unsigned int width);
			void drawItems(unsigned int width, unsigned int height);
		};

		class NavigationBar {
		public:
			NavigationBar() = default;
			NavigationBar(const NavigationBar& other) = delete;
			~NavigationBar() = default;

			NavigationBar& operator=(const NavigationBar& other) = delete;

			void draw(unsigned int width);
		};

		class DetailedView {
		public:
			DetailedView() = default;
			DetailedView(const DetailedView& other) = delete;
			~DetailedView() = default;

			DetailedView& operator=(const DetailedView& other) = delete;

			void draw(unsigned int width, unsigned int height);
		};

		class InformationPanel {
		public:
			InformationPanel() = default;
			InformationPanel(const InformationPanel& other) = delete;
			~InformationPanel() = default;

			InformationPanel& operator=(const InformationPanel& other) = delete;

			void draw(unsigned int width);
		};

		class Interface {
		public:
			Interface(Console& consoleParam);
			Interface(const Interface& other) = delete;
			~Interface() = default;

			Interface& operator=(const Interface& other) = delete;

			void runEventLoop();
		private:
			unsigned int getDiagListHeight(unsigned int totalHeight);
			unsigned int getDetailedViewHeight(unsigned int totalHeight);

			void draw();
			void drawTopBorder(unsigned int width);
			void drawSeparator(unsigned int width);
			void drawBottomBorder(unsigned int width);
		private:
			Console& console;
			DiagnosticsList diagList;
			NavigationBar navBar;
			DetailedView detailedView;
			InformationPanel infoPanel;
		};
	}
}

#endif
