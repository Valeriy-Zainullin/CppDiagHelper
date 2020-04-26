#include "ui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>


// TODO: Добавить обработку ошибок при работе с ncurses.

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
}

Console::~Console() {
	endwin(); // TODO: Добавить вывод сообщения "Failed to deinitialize ncurses." в лог при ошибке.
}


void DiagnosticsList::draw() {
	addstr("│ Строка │ Тип           │ Содержание                                    │\n");
	addstr("│────────────────────────────────────────────────────────────────────────│\n");
	addstr("│       4 Предупреждение  declaration of 'a' shadows a previous local [-W│\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
}


void NavigationBar::draw() {
	addstr("│ Подробно Замечания Подсказки Исправления      Препроцессинг Компиляция │\n");
}


void DetailedView::draw() {
	addstr("│ warning: declaration of 'a' shadows a previous local [-Wshadow]        │\n");
	addstr("│     4 |   int a = 2;                                                   │\n");
	addstr("│       |       ^                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
}


void InformationPanel::draw() {
	addstr("│ Файл: ~/Учёба/Проект/src/Console.cpp  │ Ошибок: 20 │ Предупреждений: 4 │\n");
}


Interface::Interface() {
	drawTopBorder();
	diagList.draw();
	drawSeparator();
	navBar.draw();
	drawSeparator();
	detailedView.draw();
	drawSeparator();
	infoPanel.draw();
	drawBottomBorder();
	refresh();
}

void Interface::runEventLoop() {
	getch();
}

void Interface::drawTopBorder() {
	addstr("┌────────────────────────────────────────────────────────────────────────┐\n");
}

void Interface::drawSeparator() {
	addstr("│────────────────────────────────────────────────────────────────────────│\n");
}

void Interface::drawBottomBorder() {
	addstr("└────────────────────────────────────────────────────────────────────────┘\n");
}
