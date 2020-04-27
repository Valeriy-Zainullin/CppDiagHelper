#include "ui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>
#include <cassert>
#include <cstddef>


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

void Console::getSize(unsigned int& width, unsigned int& height) {
	int signedMaxRow, signedMaxCol;
	getmaxyx(stdscr, signedMaxRow, signedMaxCol);
	width = signedMaxCol;
	height = signedMaxRow;
}


const char DiagnosticsList::HEADING_CONST_PART[] = "│ Строка │ Тип           │ Содержание ";
const std::size_t DiagnosticsList::HEADING_CONST_PART_LEN =
	sizeof(HEADING_CONST_PART) / sizeof(char);
const unsigned int DiagnosticsList::LINE_COL_WIDTH = 8;
const unsigned int DiagnosticsList::TYPE_COL_WIDTH = 15;
const unsigned int DiagnosticsList::DELIMETERS_BEFORE_CONTENT_COL = 3;
const unsigned int DiagnosticsList::CONTENT_COL_CONST_PART_LEN = 12;
const unsigned int DiagnosticsList::SEPARATOR_MIN_LEN = 3;

void DiagnosticsList::draw(unsigned int width, unsigned int height) {
	drawHeading(width);
	drawSeparator(width);
	drawItems(width, height - 2); // TODO: подумать над стилем этой строки!
}

unsigned int DiagnosticsList::getLineColWidth(unsigned int totalWidth) {
	((void) totalWidth);
	return LINE_COL_WIDTH;
}

unsigned int DiagnosticsList::getTypeColWidth(unsigned int totalWidth) {
	((void) totalWidth);
	return TYPE_COL_WIDTH;
}

unsigned int DiagnosticsList::getContentColWidth(unsigned int totalWidth) {
	const unsigned int usedSpace =
		getLineColWidth(totalWidth) + getTypeColWidth(totalWidth) + DELIMETERS_BEFORE_CONTENT_COL + 1;
	// Предполагаем, что поместится текст минимальной длины. Временное решение:
	// позже сделаем условие для вызывающей стороны. Но эту проверку всё равно оставим.
	assert(usedSpace + CONTENT_COL_CONST_PART_LEN <= totalWidth);
	return totalWidth - usedSpace;
}

void DiagnosticsList::drawHeading(unsigned int width) {
	addstr(HEADING_CONST_PART);
	for (unsigned int i = CONTENT_COL_CONST_PART_LEN; i < getContentColWidth(width); ++i) {
		addch(' ');
	}
	addstr("│");
}

void DiagnosticsList::drawSeparator(unsigned int width) {
	assert(SEPARATOR_MIN_LEN <= width);
	addstr("│");
	for (unsigned int i = 2; i < width; ++i) {
		addstr("─");
	}
	addstr("│");
}

void DiagnosticsList::drawItems(unsigned int width, unsigned int height) {
	//addstr("│       4 Предупреждение  declaration of 'a' shadows a previous local [-W│\n");
	for (unsigned int row = 0; row < height; ++row) {
		drawSeparator(width);
	}
}


void NavigationBar::draw(unsigned int width) {
	((void) width);
	addstr("│ Подробно Замечания Подсказки Исправления      Препроцессинг Компиляция │\n");
}


void DetailedView::draw(unsigned int width, unsigned int height) {
	((void) width);
	((void) height);
	addstr("│ warning: declaration of 'a' shadows a previous local [-Wshadow]        │\n");
	addstr("│     4 |   int a = 2;                                                   │\n");
	addstr("│       |       ^                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
	addstr("│                                                                        │\n");
}


void InformationPanel::draw(unsigned int width) {
	((void) width);
	addstr("│ Файл: ~/Учёба/Проект/src/Console.cpp  │ Ошибок: 20 │ Предупреждений: 4 │\n");
}


Interface::Interface(Console& consoleParam)
	: console(consoleParam) {
	draw();
}

void Interface::runEventLoop() {
	getch();
}

unsigned int Interface::getDiagListHeight(unsigned int totalHeight) {
	((void) totalHeight);
	return 10; // Временное решение. Позже придумаем распределение высот (алгоритм, формулу).
}

unsigned int Interface::getDetailedViewHeight(unsigned int totalHeight) {
	((void) totalHeight);
	return 6; // Временное решение. Позже придумаем распределение высот (алгоритм, формулу).
}


void Interface::draw() {
	unsigned int totalWidth, totalHeight;
	console.getSize(totalWidth, totalHeight);

	move(0, 0);
	drawTopBorder(totalWidth);
	diagList.draw(totalWidth, getDiagListHeight(totalHeight));
	drawSeparator(totalWidth);
	navBar.draw(totalWidth);
	drawSeparator(totalWidth);
	detailedView.draw(totalWidth, getDetailedViewHeight(totalHeight));
	drawSeparator(totalWidth);
	infoPanel.draw(totalWidth);
	drawBottomBorder(totalWidth);
	refresh();
}

void Interface::drawTopBorder(unsigned int width) {
	((void) width);
	addstr("┌────────────────────────────────────────────────────────────────────────┐\n");
}

void Interface::drawSeparator(unsigned int width) {
	((void) width);
	addstr("│────────────────────────────────────────────────────────────────────────│\n");
}

void Interface::drawBottomBorder(unsigned int width) {
	((void) width);
	addstr("└────────────────────────────────────────────────────────────────────────┘\n");
}