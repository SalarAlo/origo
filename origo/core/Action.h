#pragma once

#include <cstddef>
#include <functional>
#include <vector>

namespace Origo {

template <typename TOut = void, typename... Args>
class Action {
public:
	using ListenerID = std::size_t;
	using Callback = std::function<TOut(Args...)>;

	ListenerID add_listener(Callback cb) {
		const ListenerID id = m_next_id++;
		m_callbacks.push_back({ id, cb });
		return id;
	}

	void remove_listener(ListenerID id) {
		std::erase_if(m_callbacks, [id](const auto& entry) {
			return entry.ID == id;
		});
	}

	std::vector<TOut> invoke(Args... args) {
		std::vector<TOut> results;
		results.reserve(m_callbacks.size());
		for (auto& entry : m_callbacks) {
			results.push_back(entry.Function(args...));
		}
		return results;
	}

private:
	struct Listener {
		ListenerID ID;
		Callback Function;
	};

	std::vector<Listener> m_callbacks;
	ListenerID m_next_id {};
};

template <typename... Args>
class Action<void, Args...> {
public:
	using ListenerID = std::size_t;
	using Callback = std::function<void(Args...)>;

	ListenerID add_listener(Callback cb) {
		const ListenerID id = m_next_id++;
		m_callbacks.push_back({ id, cb });
		return id;
	}

	void remove_listener(ListenerID id) {
		std::erase_if(m_callbacks, [id](const auto& entry) {
			return entry.ID == id;
		});
	}

	void invoke(Args... args) {
		for (auto& entry : m_callbacks)
			entry.Function(args...);
	}

private:
	struct Listener {
		ListenerID ID;
		Callback Function;
	};

	std::vector<Listener> m_callbacks;
	ListenerID m_next_id {};
};

}
