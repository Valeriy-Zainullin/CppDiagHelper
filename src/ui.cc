#include "ui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>

using namespace diags::ui;

Console::Console() {
	if (std::setlocale(LC_ALL, "") == nullptr) {
		throw std::runtime_error("Failed to set locale.");
	}
	if (initscr() == nullptr) {
		throw new std::runtime_error("Failed to initialize ncurses.");
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
