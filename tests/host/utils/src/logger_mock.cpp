#include "component_interfaces/logger.hpp"

#include <cstdio>

// ─── Host-test logger implementation ──────────────────────────────────────────
//
// Platform replacement for the production logger (main/src/components/logger.cpp).
// Uses printf instead of ESP_LOGx macros so host unit tests compile without
// the ESP-IDF toolchain.
//
// This file is linked into every host test executable via CMakeLists.txt.
// ──────────────────────────────────────────────────────────────────────────────

namespace logger
{

void error(std::string_view tag, std::string_view message)
{
    std::printf("[E][%.*s] %.*s\n", (int)tag.size(), tag.data(), (int)message.size(), message.data());
}

void warn(std::string_view tag, std::string_view message)
{
    std::printf("[W][%.*s] %.*s\n", (int)tag.size(), tag.data(), (int)message.size(), message.data());
}

void info(std::string_view tag, std::string_view message)
{
    std::printf("[I][%.*s] %.*s\n", (int)tag.size(), tag.data(), (int)message.size(), message.data());
}

void debug(std::string_view tag, std::string_view message)
{
    std::printf("[D][%.*s] %.*s\n", (int)tag.size(), tag.data(), (int)message.size(), message.data());
}

void verbose(std::string_view tag, std::string_view message)
{
    std::printf("[V][%.*s] %.*s\n", (int)tag.size(), tag.data(), (int)message.size(), message.data());
}

} // namespace logger
