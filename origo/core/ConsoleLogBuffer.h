#pragma once

#include <vector>
#include <mutex>
#include <string>
#include <spdlog/spdlog.h>

namespace Origo {

struct ConsoleLogEntry {
	spdlog::level::level_enum level;
	std::string message;
};

class ConsoleLogBuffer {
public:
	void Push(ConsoleLogEntry&& entry);
	void Clear();

	std::vector<ConsoleLogEntry> Copy() const;

private:
	static constexpr size_t MaxEntries = 5000;

	std::vector<ConsoleLogEntry> m_Entries;
	mutable std::mutex m_Mutex;
};

}
