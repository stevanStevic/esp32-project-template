#include "component_interfaces/logger.hpp"

#include <esp_log.h>

// ─── Production logger implementation ─────────────────────────────────────────
//
// Wraps ESP-IDF ESP_LOGx macros. This is the only file in the business-logic
// layer that is allowed to include <esp_log.h>.
//
// The host-test counterpart is tests/host/utils/src/logger_mock.cpp which uses
// printf instead, keeping all other source files ESP-IDF-free.
// ──────────────────────────────────────────────────────────────────────────────

namespace logger
{

void error(std::string_view tag, std::string_view message)
{
    ESP_LOGE(tag.data(), "%s", message.data());
}

void warn(std::string_view tag, std::string_view message)
{
    ESP_LOGW(tag.data(), "%s", message.data());
}

void info(std::string_view tag, std::string_view message)
{
    ESP_LOGI(tag.data(), "%s", message.data());
}

void debug(std::string_view tag, std::string_view message)
{
    ESP_LOGD(tag.data(), "%s", message.data());
}

void verbose(std::string_view tag, std::string_view message)
{
    ESP_LOGV(tag.data(), "%s", message.data());
}

} // namespace logger
