/*
Условие:
Есть два класса, А и В. 
У каждого из них есть свой публичный интерфейс. 
Нужно написать новый шаблонный класс С, который в зависимости от шаблонного параметра будет выполнять разные роли:
Прокси, Медиатор или Наблюдатель
*/

#include <iostream>
#include <memory>
#include <vector>

/* The idea is from here: https://stackoverflow.com/questions/47666913/template-specialization-for-enum-values */
enum Role {
	Mediator,
	Proxy,
	Observer
};

class InterfaceB {
public:
	void virtual doSomething() noexcept = 0;
	void virtual publish(int) noexcept = 0;
};

class B : InterfaceB {
public:
	void doSomething() noexcept {
		std::cout << "B::doSomething() is called" << std::endl;
	}

	void publish(int i = 0) noexcept {
		std::cout << "B::publish() is called: " << i << std::endl;
	}
};

class A {
private:
	int my_num;
public:
	A(int number = 0): my_num(number) {}

	void doActionOnB(InterfaceB& B) noexcept {
		std::cout << "A::doActionOnB() is called" << std::endl;
		B.doSomething();
	}

	void receivePublication(int message) const {
		std::cout << "A instance with number " << my_num << " received the pulication: " << message << std::endl;
	}
};

template<Role role>
class C {};


// Real life example -- logging module
template<>
class C<Role::Proxy>: public InterfaceB {
public:
	C(B& b_to_proxy) : InterfaceB(), b_instance(b_to_proxy) {
		std::cout << "Proxy is created" << std::endl;
	}

	void doSomething() noexcept {
		std::cout << "Call of B::doSomething() was proxied" << std::endl;
		b_instance.doSomething();
	}

	void publish(int) noexcept {}

private:
	B& b_instance;
};

// Real life example -- pool of resources
template<>
class C<Role::Mediator> : public InterfaceB {
public:
	C(const std::vector<std::shared_ptr<B>>& init_pool) : InterfaceB() {
		std::cout << "Mediator is created" << std::endl;
		for (auto& x : init_pool) {
			pool.push_back(x);
		}
	}

	void doSomething() noexcept {
		std::cout << "Choosing resource automatically.." << std::endl;
		int i = rand() % pool.size();
		std::cout << "Done: " << i << " is the number of the resource to be acquired" << std::endl;
		pool[i]->doSomething();
	}

	void publish(int) noexcept {}

private:
	std::vector<std::shared_ptr<B>> pool;
};

// Real life example -- pub/sub system
template<>
class C<Role::Observer> : public InterfaceB {
public:
	C(B& b_to_observe) : InterfaceB(), b_instance(b_to_observe) {
		std::cout << "Observer is created" << std::endl;
	}

	void add_subscriber(A subscriber) {
		subscribers.push_back(subscriber);
	}

	void doSomething() noexcept {}

	void publish(int i = 0) noexcept {
		b_instance.publish(i);
		for (auto& x : subscribers) {
			x.receivePublication(i);
		}
	}

private:
	std::vector<A> subscribers;
	B& b_instance;
};

int main() {
	// TEST PROXY
	std::cout << "----------PROXY TEST------------" << std::endl;

	A a_instance1;
	B b_instance1;
	C<Role::Proxy> c_proxy(b_instance1);

	a_instance1.doActionOnB(c_proxy);

	// TEST MEDIATOR
	std::cout << "----------MEDIATOR TEST------------" << std::endl;

	A a_instance2;
	// I have a feeling that it can be done in a mush shorter way:
	std::vector<std::shared_ptr<B>> init_pool = { std::make_shared<B>(B()), std::make_shared<B>(B()), std::make_shared<B>(B()) };
	C<Role::Mediator> c_mediator(init_pool);

	a_instance2.doActionOnB(c_mediator);

	// TEST OBSERVER
	std::cout << "----------OBSERVER TEST------------" << std::endl;
	B b_instance_to_publish;
	C<Role::Observer> c_observer(b_instance_to_publish);
	A a_instance3(3), a_instance4(4), a_instance5(5);

	c_observer.add_subscriber(a_instance3);
	c_observer.add_subscriber(a_instance4);
	c_observer.add_subscriber(a_instance5);

	// Ideologically, b_instance_to_publish must do a publicationh,
	// but I think that it is wrong that B instance should contain
	// c_observer pointer inside
	// So I chose this way:
	c_observer.publish(100);

	// PAUSE
	int _;
	std::cin >> _;

	return 0;
}