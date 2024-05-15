#include <iostream>
#include <tepp/polyant.h>

struct Base
{
	virtual int foo() = 0;
	virtual std::string bar() = 0;
	virtual ~Base() = default;
};

struct One : Base
{
	int foo() override {
		return 0;
	}
	std::string bar() override {
		return "One";
	}
};

struct Two : Base
{
	int foo() override {
		return 0;
	}
	std::string bar() override {
		return "Two";
	}

	Two() = default;
	~Two() = default;
	Two(Two&& other) {
		std::cout << "two move construct\n";
	}
	Two(Two const& other) {
		std::cout << "two copy construct\n";
	}
	Two& operator=(Two&& other) {
		std::cout << "two move assign\n";
	}
	Two& operator=(Two const& other) {
		std::cout << "two copy assign\n";
	}
};

int main() {
	using T = te::polyant<Base, One, Two>;
	T polyant{};

	std::cout << polyant->bar() << "\n";
	polyant = Two();

	std::cout << polyant->bar() << "\n";
	if (auto one = polyant.getIf<One>()) {
		std::cout << "found one\n";
	}
	if (auto two = polyant.getIf<Two>()) {
		std::cout << "found two\n";
	}
	rand();
}