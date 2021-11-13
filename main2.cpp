#include <type_traits>
#include <concepts>

//#define cxwrap6(x) wrap6<[]<class A>(A a) { if constexpr (x<A>) return std::true_type{}; else return std::false_type{}; }>

template<auto Concept>
struct wrap6 {};

//template<class T, class W>
//struct test_wrap6;

//template<class T, auto Concept>
//struct test_wrap6
//{
//	using value = decltype(Concept(std::declval<T>()))::value;
//};


//template<class T, class W>
//concept concept_wrap6 = test_wrap6<T, W>::value;
//
//template<concept_wrap6<cxwrap6(std::integral)> T>
//struct oh6 {};

//oh6<int> works;
//oh6<float> not_works;

int main() {
	return 0;
}