#include "app.hpp"

#include "component_interfaces/logger.hpp"

namespace
{
static constexpr std::string_view TAG = "App";
}

void App::init()
{
    logger::info(TAG, "Initialising application...");

    // TODO: Call init/start on each component, e.g.:
    //   m_someComponent->init();

    logger::info(TAG, "Application initialised.");
}

void App::run()
{
    logger::info(TAG, "Application running.");

    // TODO: Block here waiting for a shutdown/reboot signal, e.g.:
    //   while (!m_systemControl->shouldReboot()) {
    //       vTaskDelay(pdMS_TO_TICKS(100));
    //   }
}
