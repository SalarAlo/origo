#include "origo/core/Logger.h"
#include "origo/core/ConsoleLogBuffer.h"
#include "origo/core/ConsoleSink.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Origo::Logger {

static LoggerRef s_core_logger;
static LoggerRef s_client_logger;
static std::shared_ptr<ConsoleLogBuffer> s_console_buffer;

void Init() {
	spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	s_console_buffer = std::make_shared<ConsoleLogBuffer>();
	auto console_sink = std::make_shared<ConsoleSink>(*s_console_buffer);

	s_core_logger = std::make_shared<spdlog::logger>(
	    "ORIGO",
	    spdlog::sinks_init_list { stdout_sink, console_sink });

	s_client_logger = std::make_shared<spdlog::logger>(
	    "APP",
	    spdlog::sinks_init_list { stdout_sink, console_sink });

	spdlog::register_logger(s_core_logger);
	spdlog::register_logger(s_client_logger);

	s_core_logger->set_level(spdlog::level::trace);
	s_client_logger->set_level(spdlog::level::trace);

	s_core_logger->info("Core logger initialized");
	s_client_logger->info("Client logger initialized");
}

void Shutdown() {
	spdlog::shutdown();
}

LoggerRef& GetCore() { return s_core_logger; }
LoggerRef& GetClient() { return s_client_logger; }
std::shared_ptr<ConsoleLogBuffer> GetConsoleBuffer() { return s_console_buffer; }

}
