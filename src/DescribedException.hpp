#ifndef DESCRIBED_EXCEPTION_HPP
#define DESCRIBED_EXCEPTION_HPP

#include <exception>

class DescribedException : public std::exception {
public:
	DescribedException() = delete;
	DescribedException(const char* descriptionParam) noexcept
		: description(descriptionParam) {}
	DescribedException(const DescribedException& other) = default;
	~DescribedException() = default;

	DescribedException& operator=(const DescribedException& other) = delete;

	virtual const char* what() const noexcept override {
		return description;
	}
private:
	const char* description;
};

#endif
