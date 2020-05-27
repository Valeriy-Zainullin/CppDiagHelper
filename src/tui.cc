#include "tui.h"

#include <ncurses.h>
#include <clocale>
#include <stdexcept>
#include <cassert>
#include <cstddef>


// TODO: Добавить обработку ошибок при работе с ncurses.

using namespace diags::tui;

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

void Console::write(const char* str) {
	if (addstr(str) == ERR) {
		throw std::runtime_error("Failed to write onto the screen.");
	}
}


InterfaceObject::InterfaceObject(Console& consoleParam)
	: console(consoleParam) {}

void InterfaceObject::drawTopBorder(unsigned int width) {
	drawPattern("┌", "─", "┐", width);
}

void InterfaceObject::drawEmptyLine(unsigned int width) {
	drawPattern("│", " ", "│", width);
}

void InterfaceObject::drawSeparationLine(unsigned int width) {
	drawPattern("│", "─", "│", width);
}

void InterfaceObject::drawBottomBorder(unsigned int width) {
	drawPattern("└", "─", "┘", width);
}

const unsigned int InterfaceObject::PATTERN_MIN_LEN = 3;

void InterfaceObject::drawPattern(
	const char* firstChar,
	const char* fillChar,
	const char* lastChar,
	unsigned int width
) {
	assert(PATTERN_MIN_LEN <= width);
	console.write(firstChar);
	for (unsigned int i = 2; i < width; ++i) {
		console.write(fillChar);
	}
	console.write(lastChar);
}


DiagnosticsList::DiagnosticsList(Console& consoleParam)
	: InterfaceObject(consoleParam) {}

const char DiagnosticsList::HEADING_CONST_PART[] = "│ Строка │ Тип           │ Содержание ";
const std::size_t DiagnosticsList::HEADING_CONST_PART_LEN =
	sizeof(HEADING_CONST_PART) / sizeof(char) - 1;
const unsigned int DiagnosticsList::LINE_COL_WIDTH = 8;
const unsigned int DiagnosticsList::TYPE_COL_WIDTH = 15;
const unsigned int DiagnosticsList::DELIMETERS_BEFORE_CONTENT_COL = 3;
const unsigned int DiagnosticsList::CONTENT_COL_CONST_PART_LEN = 12;

void DiagnosticsList::draw(unsigned int width, unsigned int height) {
	drawHeading(width);
	drawSeparationLine(width);
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
	console.write(HEADING_CONST_PART);
	for (unsigned int i = CONTENT_COL_CONST_PART_LEN; i < getContentColWidth(width); ++i) {
		console.write(" ");
	}
	console.write("│");
}


void DiagnosticsList::drawItems(unsigned int width, unsigned int height) {
	//console.write("│       4 Предупреждение  declaration of 'a' shadows a previous local [-W│\n");
	for (unsigned int row = 0; row < height; ++row) {
		drawEmptyLine(width);
	}
}


NavigationBar::NavigationBar(Console& consoleParam)
	: InterfaceObject(consoleParam) {}

void NavigationBar::draw(unsigned int width) {
	((void) width);
	console.write("│ Подробно Замечания Подсказки Исправления      Препроцессинг Компиляция │\n");
}


DetailedView::DetailedView(Console& consoleParam)
	: InterfaceObject(consoleParam) {}

void DetailedView::draw(unsigned int width, unsigned int height) {
	((void) width);
	((void) height);
	console.write("│ warning: declaration of 'a' shadows a previous local [-Wshadow]        │\n");
	console.write("│     4 |   int a = 2;                                                   │\n");
	console.write("│       |       ^                                                        │\n");
	drawEmptyLine(width);
	drawEmptyLine(width);
	drawEmptyLine(width);
}


InformationPanel::InformationPanel(Console& consoleParam)
	: InterfaceObject(consoleParam) {}

void InformationPanel::draw(unsigned int width) {
	((void) width);
	console.write("│ Файл: ~/Учёба/Проект/src/Console.cpp  │ Ошибок: 20 │ Предупреждений: 4 │\n");
}


// TODO: Задуматься о стиле!
Interface::Interface(Console& consoleParam)
	:
		InterfaceObject(consoleParam),
		diagList(consoleParam),
		navBar(consoleParam),
		detailedView(consoleParam),
		infoPanel(consoleParam) {
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
	drawSeparationLine(totalWidth);
	navBar.draw(totalWidth);
	drawSeparationLine(totalWidth);
	detailedView.draw(totalWidth, getDetailedViewHeight(totalHeight));
	drawSeparationLine(totalWidth);
	infoPanel.draw(totalWidth);
	drawBottomBorder(totalWidth);
	refresh();
}
