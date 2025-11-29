#pragma once

// always gotta go back to the roots (C#)
// even though c++ is now my fav language.
// iykyk

#include <functional>

namespace Origo {

template <typename TOut = void, typename... Args>
class Action {
public:
	using Callback = std::function<TOut(Args...)>;
	void AddListener(Callback cb) {
		m_Callbacks.push_back(cb);
	}

	std::vector<TOut> Invoke(Args... args) {
		std::vector<TOut> results;
		results.reserve(m_Callbacks.size());
		for (auto& cb : m_Callbacks) {
			results.push_back(cb(args...));
		}
		return results;
	}

private:
	std::vector<Callback> m_Callbacks;
};

template <typename... Args>
class Action<void, Args...> {
public:
	using Callback = std::function<void(Args...)>;

	void AddListener(Callback cb) {
		m_Callbacks.push_back(cb);
	}

	void Invoke(Args... args) {
		for (auto& cb : m_Callbacks)
			cb(args...);
	}

private:
	std::vector<Callback> m_Callbacks;
};

}
