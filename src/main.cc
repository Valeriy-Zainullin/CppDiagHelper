#include <cstdio>
#include "ui.h"

using diags::ui::Console;
using diags::ui::Interface;

int main() {
	Console console;
	Interface interface(console);
	interface.runEventLoop();
	return 0;
}