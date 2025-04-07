#pragma once

#include <iostream>
#include <string>

#define LL_OFF 0
#define LL_ERROR 1
#define LL_WARN 2
#define LL_INFO 3
#define LL_DEBUG 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LL_DEBUG
#endif // !LOG_LEVEL

#if LOG_LEVEL > LL_OFF
#include <format>
#endif

#if LOG_LEVEL >= LL_ERROR
#define ERROR(...) logger::error(std::format(__VA_ARGS__))
#else
#define ERROR(...)
#endif

#if LOG_LEVEL >= LL_WARN
#define WARN(...) logger::warn(std::format(__VA_ARGS__))
#else
#define WARN(...)
#endif

#if LOG_LEVEL >= LL_INFO
#define INFO(...) logger::info(std::format(__VA_ARGS__))
#else
#define INFO(...)
#endif

#if LOG_LEVEL >= LL_DEBUG
#define DEBUG(...) logger::debug(std::format(__VA_ARGS__))
#else
#define DEBUG(...)
#endif

/// Do not use this directly lol. Use the macros.
namespace logger {
void error(const char *msg);
void error(const std::string &msg);

void warn(const char *msg);
void warn(const std::string &msg);

void info(const char *msg);
void info(const std::string &msg);

void debug(const char *msg);
void debug(const std::string &msg);
}; // namespace logger
