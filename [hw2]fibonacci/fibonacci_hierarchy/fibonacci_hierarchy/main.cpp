#include <iostream>

class NullType {};

/////////////////// TYPELIST //////////////////////

template<typename T = NullType, typename ...U>
struct TypeList {
	using Head = T;
	using Tail = TypeList<U...>;
};

template<typename T>
struct TypeList<T> {
	using Head = T;
	using Tail = NullType;
};

// LENGTH

template <typename TList>
struct Length {
	enum { value = Length<typename TList::Tail>::value + 1 };
};

template <>
struct Length< TypeList<NullType> > {
	enum { value = 0 };
};

template <>
struct Length<NullType> {
	enum { value = 0 };
};

// TYPEAT

template<class TList, unsigned int index> struct TypeAt;

template<class Head, class ...Tail>
struct TypeAt<TypeList<Head, Tail...>, 0> {
	using Result = Head;
};

template<class Head, class ...Tail, unsigned int i>
struct TypeAt<TypeList<Head, Tail...>, i> {
	using Result = typename TypeAt<TypeList<Tail...>, i - 1>::Result;
};

// INDEXOF

template<class TList, class T> struct IndexOf;

template<class T>
struct IndexOf<TypeList<NullType>, T> {
	enum { value = -1 };
};

template<class T, class ...Tail>
struct IndexOf<TypeList<T, Tail...>, T> {
	enum { value = 0 };
};

template<class Head, class ...Tail, class T>
struct IndexOf<TypeList<Head, Tail...>, T> {
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
	enum { value = temp == -1 ? -1 : 1 + temp };
};

// APPEND

template<class TList, class T> struct Append {};

template<>
struct Append<NullType, NullType> {
	using Result = NullType;
};

template<class T>
struct Append<NullType, T> {
	using Result = TypeList<T>;
};

template<class T>
struct Append<TypeList<NullType>, T> {
	using Result = TypeList<T>;
};

template<class Head, class Tail>
struct Append<NullType, TypeList<Head, Tail>> {
	using Result = TypeList<Head, Tail>;
};

template<class ...Types, class T>
struct Append<TypeList<Types...>, T> {
	using Result = TypeList<Types..., T>; // TypeList<typename Append<TypeList<Tail...>, T>::Result>;
};

// PUSH FRONT

template <typename T, typename ...Tail>
class PushFront;

template <typename T, typename ...Tail>
class PushFront<TypeList<Tail...>, T> {
public:
	using Result = TypeList<T, Tail...>;
};

template <typename T>
class PushFront<NullType, T> {
public:
	using Result = TypeList<T>;
};

// ERASE

template <typename TypeList, typename T>
class Erase;

template <typename T>
class Erase<TypeList<>, T> {
public:
	using Result = NullType;
};

template <typename ...Tail, typename Head>
class Erase<TypeList<Head, Tail...>, Head> {
public:
	using Result = typename TypeList<Head, Tail...>::Tail;
};

template <typename Head, typename ...Tail, typename T>
class Erase<TypeList<Head, Tail...>, T> {
private:
	using temp = typename Erase<typename TypeList<Head, Tail...>::Tail, T>::Result;

public:
	using Result = std::conditional_t<std::is_same_v<temp, NullType>, TypeList<Head>, typename PushFront<temp, Head>::Result>;
};

/////////////////////// SCATTER HIERARCHY /////////////////////

template <typename T>
class MyUnit {
public:
	// static unsigned int counter;

	MyUnit() {
		std::cout << "MyUnit with type ~" << typeid(T).name() << "~ created" << std::endl;
	}
};

template <class TList, template <class> class Unit>
class GenScatterHierarchy;

template <class T1, class T2, template <class> class Unit>
class GenScatterHierarchy<TypeList<T1, T2>, Unit> :
	public GenScatterHierarchy<T1, Unit>,
	public GenScatterHierarchy<T2, Unit> {
public:
	using TList = TypeList<T1, T2>;
	using LeftBase = GenScatterHierarchy<T1, Unit>;
	using RightBase = GenScatterHierarchy<T2, Unit>;
};

template <class AtomicType, template <class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType> {
public:
	using LeftBase = Unit<AtomicType>;
};

template <template <class> class Unit>
class GenScatterHierarchy<NullType, Unit> {};

// ANOTHER IMPLEMENTATION

template<typename, template <typename> typename>
class ScatteredHierarchy;

template<template <typename> typename Unit, typename ...Types>
class ScatteredHierarchy<TypeList<Types...>, Unit> : public Unit<Types>... {
public:
	ScatteredHierarchy() {
		std::cout << "Root created" << std::endl;
	}
};

/////////////////////// LINEAR HIERARCHY /////////////////////////

template<typename, template <typename, typename> typename, class Root = NullType>
class LinearHierarchy;

template <class T1, class ...T2, template <class, class> class Unit, class Root>
class LinearHierarchy<TypeList<T1, T2...>, Unit, Root> :
	public Unit<T1, LinearHierarchy<TypeList<T2...>, Unit, Root>> {};

template <class AtomicType, template <class, class> class Unit, class Root>
class LinearHierarchy<TypeList<AtomicType>, Unit, Root> :
	public Unit<AtomicType, Root> {};

template <typename T, typename TBase>
class MyLinearUnit : public TBase {
public:
	MyLinearUnit() {
		TBase();
		std::cout << "MyLinearUnit with type ~" << typeid(T).name() << "~ created" << std::endl;
	}
};

/////////////////////// FIBONACCI /////////////////////

template<size_t N>
struct Fibonacci : std::integral_constant < size_t, Fibonacci<N - 1>::value + Fibonacci<N - 2>::value > {};

template<> struct Fibonacci<2> : std::integral_constant<size_t, 1> {};
template<> struct Fibonacci<1> : std::integral_constant<size_t, 1> {};
// template<> struct Fibonacci<0> : std::integral_constant<size_t, 0> {};

/////////////////////// LECTURE TASK 2 ////////////////////////

// Special Unit to wrap Linear Hierarchy
template <typename T>
class LHUnit {
public:
	LHUnit() {
		std::cout << "LHUnit with type ~" << typeid(T).name() << "~ created" << std::endl;
	}
};

// Class for generating TypeList of TypeLists using Fibonacci, e.g.
// TList<int, char, double, string, float> --> TList<TList<int>, TList<char>, TList<double, string>, TList<float>>

template<typename TList>
class FibonacciTypeList;

template<typename ...Tail>
class FibonacciTypeList<TypeList<Tail...>> {
public:
	using Result = TypeList<TypeList<int>, TypeList<char>, TypeList<double, std::string>, TypeList<float>>; // for testing
};


/*
template<typename Head, unsigned int i, typename ...Tail>
class FibonacciTypeList<TypeList<Head, Tail...>, 1> {
public:
	using Result = TypeList<Head>;
};
template<typename Head, typename ...Tail>
class FibonacciTypeList<TypeList<Head, Tail...>, 1> {
public:
	using Result = TypeList<Head>;
};
template<typename Head, unsigned int i, typename ...Tail>
class FibonacciTypeList {
public:
	....
};
*/

// Class to make TypeList of LinearHierarchies from FibonacciTypeList, e.g.
// TList<TList<int>, TList<char>, TList<double, string>, TList<float>> --> TList<LH<int>, LH<char>, LH<double, string>, LH<float>>
template<typename FibTList>
class FibonacciLinearHierarchyList;

template<typename ...Tail>
class FibonacciLinearHierarchyList<TypeList<Tail...>> {
public:
	using Result = TypeList<LinearHierarchy<Tail, MyLinearUnit>...>;
};

// Class that builds required in Task 2 object, e.g.
// TList<LH<int>, LH<char>, LH<double, string>, LH<float>> --> ScatteredHierarchy<LH<int>, LH<char>, LH<double, string>, LH<float>>
template<typename FibTList>
class FibonacciHierarchy;

template<typename ...Tail>
class FibonacciHierarchy<TypeList<Tail...>> {
public:
	using Result = ScatteredHierarchy< FibonacciLinearHierarchyList<TypeList<TypeList<int>, TypeList<char>, TypeList<double, std::string>, TypeList<float>>>::Result, LHUnit >;  // for testing
};

/////////////////// ADDITIONAL TEST ////////////////////////
template class LinearHierarchy<TypeList<int, char, double>, MyLinearUnit, NullType>;
////////////////////////////////////////////////////////////

int main() {
	// TEST TYPELIST
	using FourElementList = TypeList<char, signed char, unsigned char, int>;
	using EmptyList = TypeList<>;
	TypeList<int, int, char, char, double> tlist;

	std::cout << "------TYPELIST TEST:------" << std::endl;
	std::cout << "EmptyList = " << typeid(EmptyList).name() << std::endl;
	std::cout << "FourElementList = " << typeid(FourElementList).name() << std::endl;
	// std::cout << typeid(TypeList<TypeList<int, double>, char>).name() << std::endl;

	std::cout << "Length(EmptyList) = " << Length<EmptyList>::value << std::endl;
	std::cout << "Length(FourElementList) = " << Length<FourElementList>::value << std::endl;

	std::cout << "TypeAt(EmptyList, 0) = " << typeid(TypeAt<EmptyList, 0>::Result).name() << std::endl;
	std::cout << "TypeAt(FourElementList, 0) = " << typeid(TypeAt<FourElementList, 0>::Result).name() << std::endl;
	std::cout << "TypeAt(FourElementList, 2) = " << typeid(TypeAt<FourElementList, 2>::Result).name() << std::endl;

	std::cout << "IndexOf(EmptyList, int) = " << IndexOf<EmptyList, int>::value << std::endl;
	std::cout << "IndexOf(FourElementList, int) = " << IndexOf<FourElementList, int>::value << std::endl;
	std::cout << "IndexOf(FourElementList, char) = " << IndexOf<FourElementList, char>::value << std::endl;
	std::cout << "IndexOf(FourElementList, std::string) = " << IndexOf<FourElementList, std::string>::value << std::endl;

	std::cout << "Append(EmptyList, int) = " << typeid(Append<EmptyList, int>::Result).name() << std::endl;
	std::cout << "Append(FourElementList, double) = " << typeid(Append<FourElementList, double>::Result).name() << std::endl;
	std::cout << "Append(FourElementList, std::string) = " << typeid(Append<FourElementList, std::string>::Result).name() << std::endl;

	std::cout << "PushFront(EmptyList, int) = " << typeid(PushFront<EmptyList, int>::Result).name() << std::endl;
	std::cout << "PushFront(FourElementList, double) = " << typeid(PushFront<FourElementList, double>::Result).name() << std::endl;
	std::cout << "PushFront(FourElementList, std::string) = " << typeid(PushFront<FourElementList, std::string>::Result).name() << std::endl;

	// std::cout << "Erase(EmptyList, int) = " << typeid(Erase<EmptyList, int>::Result).name() << std::endl;
	std::cout << "Erase(FourElementList, int) = " << typeid(Erase<FourElementList, int>::Result).name() << std::endl;
	std::cout << "Erase(FourElementList, unsigned char) = " << typeid(Erase<FourElementList, unsigned char>::Result).name() << std::endl;
	//std::cout << "Erase(FourElementList, std::string) = " << typeid(Erase<FourElementList, std::string>::Result).name() << std::endl;

	// TEST FIBONACCI
	std::cout << "------FIBONACCI TEST------" << std::endl;
	std::cout << "Fib(1) = " << Fibonacci<1>::value << std::endl;
	std::cout << "Fib(5) = " << Fibonacci<5>::value << std::endl;
	std::cout << "Fib(10) = " << Fibonacci<10>::value << std::endl;

	// TEST SCATTERED HIERARCHY
	std::cout << "------SCATTERED HIERARCHY TEST------" << std::endl;
	ScatteredHierarchy<TypeList<int, char, double>, MyUnit> sh;

	// TEST LINEAR HIERARCHY
	std::cout << "------LINEAR HIERARCHY TEST------" << std::endl;
	using MyLH = LinearHierarchy<TypeList<int, char, double>, MyLinearUnit, NullType>;
	MyLH a;

	// TEST TASK 2
	using FiveElementList = TypeList<int, char, double, std::string, float>;

	// TEST FIBBONACCI TYPELIST
	std::cout << "------FIBONACCI TYPELIST TEST------" << std::endl;
	std::cout << "FiveElementList = " << typeid(FiveElementList).name() << std::endl;
	using FibTList = FibonacciTypeList<FiveElementList>::Result;
	std::cout << "FibonacciTypeList(FiveElementList) = " << typeid(FibTList).name() << std::endl;

	// TEST FIBONACCI HIERARCHY
	std::cout << "------TASK 2 TEST------" << std::endl;
	using Task2_Solution = FibonacciHierarchy<FiveElementList>::Result;
	Task2_Solution solution;

	// PAUSE
	int _;
	std::cin >> _;

	return 0;
}