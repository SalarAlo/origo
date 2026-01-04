#pragma once
#include <spdlog/sinks/base_sink.h>
#include "ConsoleLogBuffer.h"

namespace Origo {

class ConsoleSink final
    : public spdlog::sinks::base_sink<spdlog::details::null_mutex> {
public:
	explicit ConsoleSink(ConsoleLogBuffer& buffer)
	    : m_Buffer(buffer) { }

protected:
	void sink_it_(const spdlog::details::log_msg& msg) override {
		ConsoleLogEntry entry;
		entry.level = msg.level;
		entry.message.assign(msg.payload.begin(), msg.payload.end());
		m_Buffer.Push(std::move(entry));
	}

	void flush_() override { }

private:
	ConsoleLogBuffer& m_Buffer;
};

}
