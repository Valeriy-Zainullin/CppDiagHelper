#include "TextUI.hpp"

#include <ncurses.h>
#include <clocale>

namespace TextUI {
	void initConsole() {
		if (std::setlocale(LC_ALL, "") == nullptr) {
			throw UIException("Failed to set locale.");
		}
		if (initscr() == nullptr) {
			throw new UIException("Failed to initialize ncurses.");
		}
		noecho();
		keypad(stdscr, true);
		cbreak();
		printw("Hello world!");
		getch();
	}

	void deinitConsole() {
		if (endwin() == ERR) {
			throw new UIException("Failed to deinitialize ncurses.");
		}
	}
}
