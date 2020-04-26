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
	}
}

#endif
