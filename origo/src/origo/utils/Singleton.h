#pragma once

namespace Origo {

template <typename T>
class Singleton {
public:
	static T& get_instance() {
		static T instance = [] {
			T obj;
			return obj;
		}();

		return instance;
	}

	~Singleton() = default;

protected:
	Singleton() = default;

	Singleton(const Singleton&) = default;
	Singleton& operator=(const Singleton&) = default;

	Singleton(Singleton&&) = default;
	Singleton& operator=(Singleton&&) = default;
};

}
