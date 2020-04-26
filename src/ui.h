#ifndef UI_H
#define UI_H

namespace diags {
	namespace ui {
		class Console {
		public:
			Console();
			Console(const Console& other) = delete;
			~Console();

			Console& operator=(const Console& other) = delete;
		};

		class DiagnosticsList {
		public:
			DiagnosticsList() = default;
			DiagnosticsList(const DiagnosticsList& other) = delete;
			~DiagnosticsList() = default;

			DiagnosticsList& operator=(const DiagnosticsList& other) = delete;

			void draw();
		};

		class NavigationBar {
		public:
			NavigationBar() = default;
			NavigationBar(const NavigationBar& other) = delete;
			~NavigationBar() = default;

			NavigationBar& operator=(const NavigationBar& other) = delete;

			void draw();
		};

		class DetailedView {
		public:
			DetailedView() = default;
			DetailedView(const DetailedView& other) = delete;
			~DetailedView() = default;

			DetailedView& operator=(const DetailedView& other) = delete;

			void draw();
		};

		class InformationPanel {
		public:
			InformationPanel() = default;
			InformationPanel(const InformationPanel& other) = delete;
			~InformationPanel() = default;

			InformationPanel& operator=(const InformationPanel& other) = delete;

			void draw();
		};

		class Interface {
		public:
			Interface();
			Interface(const Interface& other) = delete;
			~Interface() = default;

			Interface& operator=(const Interface& other) = delete;

			void runEventLoop();
		private:
			void drawTopBorder();
			void drawSeparator();
			void drawBottomBorder();
		private:
			DiagnosticsList diagList;
			NavigationBar navBar;
			DetailedView detailedView;
			InformationPanel infoPanel;
		};
	}
}

#endif
