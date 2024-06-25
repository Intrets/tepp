#include <iostream>
#include <tepp/polyant.h>
#include <tepp/rc_object.h>

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

	Two() {
		std::cout << "construct\n";
	}
	~Two() {
		std::cout << "destruct\n";
	}
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
	te::rc_object<Two> object{};
	{
		//te::rc_object<Two> test = object;
		//test->foo();
	}

	te::rc_object_ref<Two> ref = object;
	auto access = ref.access();
	rand();
}