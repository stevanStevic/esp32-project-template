#include <memory>

#include "app.hpp"

// ─── Composition Root ─────────────────────────────────────────────────────────
//
// This is the only place in the codebase that knows about concrete
// implementations. All other code depends on abstract interfaces defined in
// component_interfaces/.
//
// Wiring steps:
//   1. Instantiate ESP-IDF wrapper components (concrete implementations)
//   2. Create inter-task queues
//   3. Pass everything into business-logic components via constructor injection
//   4. Hand off to App::init() and App::run()
//
// TODO: Replace with your actual component wiring.
// ──────────────────────────────────────────────────────────────────────────────

extern "C" void app_main()
{
    // TODO: Instantiate concrete ESP-IDF wrapper components, e.g.:
    //   auto gpio  = std::make_shared<GpioHandler>(...);
    //   auto queue = std::make_shared<RtosQueue<MyCommand>>(10);

    // TODO: Instantiate business-logic components with injected deps, e.g.:
    //   auto manager = std::make_shared<MyManager>(gpio, queue);

    // TODO: Wire into App, e.g.:
    //   auto app = std::make_unique<App>(manager);
    //   app->init();
    //   app->run();
}
