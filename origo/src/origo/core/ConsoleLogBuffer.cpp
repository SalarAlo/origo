#include "origo/core/ConsoleLogBuffer.h"

namespace Origo {

void ConsoleLogBuffer::Push(ConsoleLogEntry&& entry) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	m_Entries.emplace_back(std::move(entry));

	if (m_Entries.size() > MaxEntries)
		m_Entries.erase(m_Entries.begin());
}

void ConsoleLogBuffer::Clear() {
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_Entries.clear();
}

std::vector<ConsoleLogEntry> ConsoleLogBuffer::Copy() const {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Entries;
}

}
