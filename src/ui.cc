#include "ui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>

using namespace diags::ui;

Console::Console() {
	if (std::setlocale(LC_ALL, "ru_RU.UTF-8") == nullptr) {
		throw std::runtime_error("Failed to set locale.");
	}
	if (initscr() == nullptr) {
		throw std::runtime_error("Failed to initialize ncurses.");
	}
	noecho();
	keypad(stdscr, true);
	cbreak();
	addstr(
		"┌────────────────────────────────────────────────────────────────────────┐\n"
		"│ Строка │ Тип           │ Содержание                                    │\n"
		"│────────────────────────────────────────────────────────────────────────│\n"
		"│       4 Предупреждение  declaration of 'a' shadows a previous local [-W│\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│────────────────────────────────────────────────────────────────────────│\n"
		"│ Подробно Замечания Подсказки Исправления      Препроцессинг Компиляция │\n"
		"│────────────────────────────────────────────────────────────────────────│\n"
		"│ warning: declaration of 'a' shadows a previous local [-Wshadow]        │\n"
		"│     4 |   int a = 2;                                                   │\n"
		"│       |       ^                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│                                                                        │\n"
		"│────────────────────────────────────────────────────────────────────────│\n"
		"│ Файл: ~/Учёба/Проект/src/Console.cpp  │ Ошибок: 20 │ Предупреждений: 4 │\n"
		"└────────────────────────────────────────────────────────────────────────┘\n"
	);
	refresh();
	getch();
}

Console::~Console() {
	endwin(); // TODO: Добавить вывод сообщения "Failed to deinitialize ncurses." в лог при ошибке.
}
