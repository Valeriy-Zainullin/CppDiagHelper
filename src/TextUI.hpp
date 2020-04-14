#ifndef TEXT_UI_HPP
#define TEXT_UI_HPP

#include "DescribedException.hpp"

namespace TextUI {
	class UIException : public DescribedException {
	public:
		using DescribedException::DescribedException;
	};

	void initConsole();
	void deinitConsole();
}

#endif
