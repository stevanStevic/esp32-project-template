#ifndef MAIN_INCLUDE_APP_HPP_
#define MAIN_INCLUDE_APP_HPP_

// ─── App ──────────────────────────────────────────────────────────────────────
//
// The application orchestrator. Owns shared_ptr references to all top-level
// business-logic components. Responsible for initialisation sequencing and
// running the main event loop (or blocking until a shutdown signal is received).
//
// Dependencies are injected through the constructor — App itself does NOT
// instantiate any concrete classes.
//
// TODO: Add your component shared_ptrs as constructor parameters and members.
// ──────────────────────────────────────────────────────────────────────────────

class App
{
public:
    // TODO: Add std::shared_ptr<SomeComponentIf> parameters as needed, e.g.:
    //   explicit App(std::shared_ptr<SomeComponentIf> someComponent);
    App() = default;

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    ~App() = default;

    /// @brief Initialise all components. Call once before run().
    void init();

    /// @brief Enter the main run loop. Blocks until shutdown.
    void run();
};

#endif // MAIN_INCLUDE_APP_HPP_
