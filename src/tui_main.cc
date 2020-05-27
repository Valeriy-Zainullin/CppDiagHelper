#include <cstdio>
#include "tui.h"

using diags::tui::Console;
using diags::tui::Interface;

int main() {
	Console console;
	Interface interface(console);
	interface.runEventLoop();
	return 0;
}
