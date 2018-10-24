#include <iostream>

using std::cout;

class NullType {};

template<typename T=NullType, typename ...U>
struct TypeList {
    using head = T;
    using tail = TypeList<U...>;
};

template<typename T>
struct TypeList<T> {
    using head = T;
    using tail = NullType;
};

// LENGTH

template <typename TypeList>
struct Length{
    enum { value = Length<typename TypeList::tail>::value + 1};
};

template <>
struct Length<TypeList<NullType>>{
    enum { value = 0 };
};

template <>
struct Length<NullType>{
    enum { value = 0 };
};

// TYPEAT

//template<class Head, class Tail, unsigned int i>
//struct TypeAt{};
//
//
//template<class Head, class Tail, unsigned int i>
//struct TypeAt<TypeList<Head, Tail>, i> {
//    using Result = TypeAt<Tail, i-1>::Result;
//};
//
//template<class Head, class Tail>
//struct TypeAt<TypeList<Head, Tail>, 0> {
//    using Result = Head;
//};
//
//
//// INDEXOF
//
//template<class Head, class Tail, class T>
//struct IndexOf{
//};
//
//template<class T>
//struct IndexOf<NullType, T>{
//    enum {value = -1};
//};
//
//template<class Head, class Tail, class T>
//struct IndexOf<TypeList<Head, Tail>, T>{
//private:
//    enum { temp = IndexOf<Tail, T>::value };
//public:
//    enum { value = (temp == -1) ? -1 : 1 + temp};
//};

///////////////////////////////////////////////////////////////////////////////

using CharList = TypeList<char, signed char, unsigned char, int>;
using EmptyList = TypeList<>;

int main() {
    // Creation check
    TypeList<int, int, char, char, double> a;
    // Length check
    cout << Length<EmptyList>::value << std::endl;
    cout << Length<CharList>::value << std::endl;

    return 0;
}