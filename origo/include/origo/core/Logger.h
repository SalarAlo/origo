#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Origo::Logger {

using LoggerRef = std::shared_ptr<spdlog::logger>;

void Init();
void Shutdown();

LoggerRef& GetCore();
LoggerRef& GetClient();

}

#define ORG_CORE_TRACE(...) ::Origo::Logger::GetCore()->trace(__VA_ARGS__)
#define ORG_CORE_INFO(...) ::Origo::Logger::GetCore()->info(__VA_ARGS__)
#define ORG_CORE_WARN(...) ::Origo::Logger::GetCore()->warn(__VA_ARGS__)
#define ORG_CORE_ERROR(...) ::Origo::Logger::GetCore()->error(__VA_ARGS__)
#define ORG_CORE_CRITICAL(...) ::Origo::Logger::GetCore()->critical(__VA_ARGS__)

#define ORG_TRACE(...) ::Origo::Logger::GetClient()->trace(__VA_ARGS__)
#define ORG_INFO(...) ::Origo::Logger::GetClient()->info(__VA_ARGS__)
#define ORG_WARN(...) ::Origo::Logger::GetClient()->warn(__VA_ARGS__)
#define ORG_ERROR(...) ::Origo::Logger::GetClient()->error(__VA_ARGS__)
#define ORG_CRITICAL(...) ::Origo::Logger::GetClient()->critical(__VA_ARGS__)
