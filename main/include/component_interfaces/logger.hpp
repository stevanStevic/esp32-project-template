#ifndef MAIN_INCLUDE_COMPONENT_INTERFACES_LOGGER_HPP_
#define MAIN_INCLUDE_COMPONENT_INTERFACES_LOGGER_HPP_

#include <string_view>

// ─── Logger ───────────────────────────────────────────────────────────────────
//
// Free-function logging interface. Business-logic components call these
// functions and never include ESP-IDF logging headers directly.
//
// Two implementations exist:
//   - src/components/logger.cpp   (production: wraps ESP_LOGx macros)
//   - tests/host/utils/src/logger_mock.cpp  (host tests: uses printf)
//
// This allows any component that only includes this header to be compiled and
// tested on a host machine without the ESP-IDF toolchain.
// ──────────────────────────────────────────────────────────────────────────────

namespace logger
{

void error(std::string_view tag, std::string_view message);
void warn(std::string_view tag, std::string_view message);
void info(std::string_view tag, std::string_view message);
void debug(std::string_view tag, std::string_view message);
void verbose(std::string_view tag, std::string_view message);

} // namespace logger

#endif // MAIN_INCLUDE_COMPONENT_INTERFACES_LOGGER_HPP_
