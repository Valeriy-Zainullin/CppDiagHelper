#include <iostream>

void getNumber() {}

void setName(const char* str) {}

int main() {
	std::cout << getNumber();
	setName(std::string("Имя"));
	return 0;
}
