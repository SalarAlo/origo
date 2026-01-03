#include "origo/core/Logger.h"
#include "origo/core/ConsoleLogBuffer.h"
#include "origo/core/ConsoleSink.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Origo::Logger {

static LoggerRef s_CoreLogger;
static LoggerRef s_ClientLogger;
static std::shared_ptr<ConsoleLogBuffer> s_ConsoleBuffer;

void Init() {
	spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	s_ConsoleBuffer = std::make_shared<ConsoleLogBuffer>();
	auto console_sink = std::make_shared<ConsoleSink>(*s_ConsoleBuffer);

	s_CoreLogger = std::make_shared<spdlog::logger>(
	    "ORIGO",
	    spdlog::sinks_init_list { stdout_sink, console_sink });

	s_ClientLogger = std::make_shared<spdlog::logger>(
	    "APP",
	    spdlog::sinks_init_list { stdout_sink, console_sink });

	spdlog::register_logger(s_CoreLogger);
	spdlog::register_logger(s_ClientLogger);

	s_CoreLogger->set_level(spdlog::level::trace);
	s_ClientLogger->set_level(spdlog::level::trace);

	s_CoreLogger->info("Core logger initialized");
	s_ClientLogger->info("Client logger initialized");
}

void Shutdown() {
	spdlog::shutdown();
}

LoggerRef& GetCore() { return s_CoreLogger; }
LoggerRef& GetClient() { return s_ClientLogger; }
std::shared_ptr<ConsoleLogBuffer> GetConsoleBuffer() { return s_ConsoleBuffer; }

}
