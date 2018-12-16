#include <iostream>
#include <fstream>

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


// SIZEOF

template <typename TypeList>
class SizeOf;

template <>
class SizeOf<TypeList<>> {
public:
	enum { value = 0 };

};

template <typename Head, typename ...Tail>
class SizeOf<TypeList<Head, Tail...>> {
public:
	enum { value = sizeof(Head) + SizeOf<TypeList<Tail...>>::value};
};


/////////////////// READER /////////////////////

template<typename TList>
class Reader {
public:
	Reader(const std::string& filename) : infile_(filename, std::ios::binary), position_(0) {
		// RAII
		total_typelist_size_ = SizeOf<TList>::value;
	}

	void* readNextLine() {

		infile_.seekg(position_);  // set the current position

		delete curr_ptr_;  // delete preceeding row
		curr_ptr_ = operator new(total_typelist_size_);
		void *begin = curr_ptr_;

		infile_.read(curr_ptr_, total_typelist_size_);
		position_ = infile_.tellg(); // get the current position

		/*
		for (int i : Length<TList>) {
			type_size = std::sizeof(TypeAt<TList, i>);

			infile.read(ptr_, type_size); // read the structure

			position_ = infile.tellg(); // get the current position
		}

		begin_position = curr_pos;
		file.seekg(std::sizeof(curr_type), std::ios_base::cur);
		end_position += std::sizeof(curr_type);
		*/

		return begin;  // or just curr_ptr_?
	}

	~Reader() {
		infile_.close();
	}

private:
	std::size_t total_typelist_size_;
	std::ifstream infile_;
	std::size_t position_;
	void *curr_ptr_;
};

int main() {
	const std::string filename = "./test.txt";

	Reader<TypeList<int, char, int, double>> reader(filename);

	for (int i = 0; i < 3; i++) {
		reader.readNextLine();
	}

	return 0;
}