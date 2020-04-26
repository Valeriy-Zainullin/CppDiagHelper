#include "ui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>

using namespace diags::ui;

Console::Console() {
	if (initscr() == nullptr) {
		throw std::runtime_error("Failed to initialize ncurses.");
	}
	noecho();
	keypad(stdscr, true);
	cbreak();
	printw("Hello world!");
	getch();
}

Console::~Console() {
	endwin(); // TODO: Добавить вывод сообщения "Failed to deinitialize ncurses." в лог при ошибке.
}
