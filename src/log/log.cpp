#include "log.h"
#include <iostream>

#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define ESCAPE "\033[0m"

#if LOG_LEVEL >= LOG_LEVEL_INFO
void logger::info(const char *msg) {
  // in green
  std::cout << GREEN << "INFO: " << ESCAPE << msg << std::endl;
};
#else
void logger::info(const char *msg) {}
#endif
void logger::info(const std::string &msg) { logger::info(msg.c_str()); }

#if LOG_LEVEL >= LOG_LEVEL_WARN
void logger::warn(const char *msg) {
  // in yellow
  std::cout << YELLOW << "WARN: " << ESCAPE << msg << std::endl;
}
#else
void logger::warn(const char *msg) {}
#endif
void logger::warn(const std::string &msg) { logger::warn(msg.c_str()); }

#if LOG_LEVEL >= LOG_LEVEL_ERROR
void logger::error(const char *msg) {
  // in red
  std::cout << RED << "ERROR: " << ESCAPE << msg << std::endl;
}
#else
void logger::error(const char *msg) {}
#endif
void logger::error(const std::string &msg) { logger::error(msg.c_str()); }

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
void logger::debug(const char *msg) {
  // in blue
  std::cout << BLUE << "DEBUG: " << ESCAPE << msg << std::endl;
}
#else
void logger::debug(const char *msg) {}
#endif
void logger::debug(const std::string &msg) { logger::debug(msg.c_str()); }
