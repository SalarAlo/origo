#pragma once

#include <functional>

namespace Origo {

template <typename TOut = void, typename... Args>
class Action {
public:
	using Callback = std::function<TOut(Args...)>;
	void add_listener(Callback cb) {
		m_callbacks.push_back(cb);
	}

	std::vector<TOut> invoke(Args... args) {
		std::vector<TOut> results;
		results.reserve(m_callbacks.size());
		for (auto& cb : m_callbacks) {
			results.push_back(cb(args...));
		}
		return results;
	}

private:
	std::vector<Callback> m_callbacks;
};

template <typename... Args>
class Action<void, Args...> {
public:
	using Callback = std::function<void(Args...)>;

	void add_listener(Callback cb) {
		m_callbacks.push_back(cb);
	}

	void invoke(Args... args) {
		for (auto& cb : m_callbacks)
			cb(args...);
	}

private:
	std::vector<Callback> m_callbacks;
};

}
