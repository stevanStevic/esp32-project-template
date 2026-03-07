# Agent Usage Guide

Step-by-step guide for using the AI agentic workflows to develop and evolve ESP-IDF projects from this
template.

---

## Prerequisites

- VS Code with GitHub Copilot extension (for Copilot workflow)
- OR Claude Code CLI (`npm install -g @anthropic-ai/claude-code`) for Claude workflow
- Devcontainer running (opens automatically in VS Code)
- A rough idea of what your ESP32 device should do (greenfield) OR an existing project to modify (CR)

---

## Workflows at a Glance

**Greenfield — new project from scratch:**
```
Phase 1: Decompose Requirements    ← Input: business desc + hardware info
           ↓ user confirms
Phase 2: Design Architecture       ← Input: approved requirements
           ↓ user confirms
Phase 3: Scaffold Project          ← Generates all code files
           ↓
Phase 4: Add Component (repeat)    ← Incremental additions
           ↓
Phase 5: Generate Tests (repeat)   ← Keep coverage up
           ↓
Phase 6: CI Setup                  ← One-time finalisation
```

**Change request / incremental feature — existing project:**
```
Phase 7: Analyze Change Request    ← Reads existing code, maps blast radius
           ↓ user confirms impact report
Phase 8: Add Feature               ← Implements change with minimal footprint
```

Phases 1, 2, and 7 have **confirmation checkpoints** — the agent stops and asks you to verify its output
before writing any code. This is intentional.

### Which workflow to use?

| Situation | Use |
|---|---|
| New project, no code yet | Phases 1 → 2 → 3 → 4 → 5 → 6 |
| New capability added to existing project, spans multiple components | Phases 7 → 8 |
| New component added to existing project, well-understood | Phase 4 directly |
| Behavioral change to an existing component | Phases 7 → 8 |
| Bug fix in a known location | Phase 8 directly (no analysis needed) |
| Tests missing or stale | Phase 5 directly |

---

## Phase 1: Requirements Decomposition

### What you provide

High-level description of:
- **Business purpose**: What does this device do? Who uses it?
- **Hardware**: ESP32 variant, which peripherals are connected, which communication buses
- **Constraints**: timing requirements, memory budget, power budget, safety requirements
- **External interface**: How does the outside world interact with the firmware?
  (WiFi REST API, BLE GATT, UART command protocol, autonomous logging only, etc.)

You don't need to have all hardware details to start. The agent will work with what you have and identify gaps.

### Example input

```
Build a temperature monitoring device on ESP32-S3.
Hardware:
  - ESP32-S3-WROOM-1-N16R8 (16MB flash, 8MB PSRAM)
  - BME280 on I2C bus (address 0x76) for temperature/humidity/pressure
  - RGB LED on GPIO 48
  - USB-UART for command input (115200 baud)

The device should read sensor every 10 seconds and log to UART.
A user can request the latest reading by sending "GET_READING\n" on UART.
The device should respond with a JSON line: {"temp": 23.4, "hum": 65.2, "press": 1013.2}

Constraints: reading latency < 100ms for UART commands
```

### What the agent produces

```
Functional Requirements:
  FR-001: The device shall read BME280 sensor every 10 seconds
  FR-002: The device shall log readings to UART as JSON lines
  FR-003: The device shall respond to "GET_READING\n" within 100ms

Hardware Abstraction Requirements:
  HAL-001: I2cBusIf — wraps i2c_master_transmit_receive — methods: write(), read()
  HAL-002: GpioHandlerIf — wraps gpio_set_level — methods: setLevel()
  HAL-003: UartHandlerIf — wraps uart_write_bytes/uart_read_bytes — methods: writeLine(), readLine()

Component Identification:
  SensorManager — reads BME280, caches latest reading
    Depends on: I2cBusIf
  LedController — manages RGB LED state
    Depends on: GpioHandlerIf
  UartCommandHandler — listens for commands, sends responses
    Depends on: UartHandlerIf, SensorManager
```

**Confirm before proceeding**.

---

## Phase 2: Architecture Design

### What you provide

Confirmation of Phase 1 output (or corrections).

### What the agent produces

- Complete interface list with method signatures
- Concrete component list with constructor parameters
- Queue topology (if inter-task messaging needed)
- Mermaid class diagram
- Composition root pseudocode
- Requirement traceability table

### Example output excerpt

```
Interface List:
  I2cBusIf (component_interfaces/i2c_bus_if.hpp)
    Methods:
      - bool write(uint8_t address, std::span<const uint8_t> data)
      - bool read(uint8_t address, std::span<uint8_t> data)
    Implemented by: I2cBus

  SensorDataIf (component_interfaces/sensor_data_if.hpp)
    Methods:
      - SensorReading getLatestReading() const
    Implemented by: SensorManager

Component List:
  I2cBus : I2cBusIf
    Responsibility: Wraps ESP-IDF I2C master driver
    Constructor args: i2c_port_t port, int sda, int scl
    ESP-IDF APIs: i2c_master_init, i2c_master_transmit_receive

  SensorManager : SensorDataIf
    Responsibility: Periodically reads BME280, caches last reading
    Constructor args: shared_ptr<I2cBusIf> i2c, shared_ptr<TaskRunner> taskRunner
```

**Confirm before code generation**.

---

## Phase 3: Full Project Scaffold

### What the agent generates

Given confirmed architecture, generates ALL files:

```
main/include/component_interfaces/i2c_bus_if.hpp
main/include/component_interfaces/sensor_data_if.hpp
main/include/component_interfaces/uart_handler_if.hpp
main/include/component_interfaces/gpio_handler_if.hpp
main/include/components/i2c_bus.hpp
main/include/components/sensor_manager.hpp
main/include/components/uart_command_handler.hpp
main/include/components/led_controller.hpp
main/src/components/i2c_bus.cpp
main/src/components/sensor_manager.cpp
main/src/components/uart_command_handler.cpp
main/src/components/led_controller.cpp
main/main.cpp          (composition root, fully wired)
main/include/app.hpp   (updated with all deps)
main/src/app.cpp       (init/run implemented)
main/CMakeLists.txt    (all sources listed)

tests/host/sensor_manager_tests.cpp
tests/host/uart_command_handler_tests.cpp
tests/host/led_controller_tests.cpp
tests/host/CMakeLists.txt  (all test targets)
```

### After scaffold

```bash
idf.py build                          # Should compile clean
./scripts/run_all_tests.sh --host     # Host tests should pass
```

---

## Phase 4: Add Component (Incremental)

Use this phase when adding a new capability to an existing project.

### Example input

```
Add a WatchdogManager component.
Responsibility: Resets the hardware watchdog every 2 seconds in a background task.
Dependencies: TaskRunner
ESP-IDF APIs: esp_task_wdt_add, esp_task_wdt_reset
Interface method: void start() — begins feeding the watchdog in background
```

The agent will:
1. Create `component_interfaces/watchdog_manager_if.hpp`
2. Create `components/watchdog_manager.hpp`  
3. Create `src/components/watchdog_manager.cpp`
4. Create `tests/host/watchdog_manager_tests.cpp`
5. Update `main/CMakeLists.txt`, `tests/host/CMakeLists.txt`
6. Update `main.cpp` composition root wiring
7. Update `App` to init the new component

---

## Phase 5: Generate Tests

Use this phase to add or update tests without adding new components.

### Example input

```
Generate tests for SensorManager.
Test cases needed:
  - Happy path: successful I2C read returns correct temperature
  - Error path: I2C read failure returns error code
  - Timeout: no I2C response within 100ms returns timeout error

Also add E2E test that verifies the device logs a temperature reading within 15 seconds of boot.
External interface: serial log output (dut.expect pattern).
```

---

## Phase 6: CI Setup

One-time setup to finalise the CI workflow for your project.

### Example input

```
Project name: TemperatureMonitor
Team: @alice, @bob
ESP-IDF version: v5.5.2
Secure Boot: yes (for production)
S3 bucket: my-company-firmware-keys
```

---

---

## Phase 7: Analyze Change Request

Use this before touching any code on an existing project. The agent reads the affected files first, maps
dependencies, and produces a confirmed impact report. Nothing is written until you approve.

### Copilot: attach the prompt

In Copilot Chat, click the paperclip icon → **Prompt...** → select
`.github/copilot/prompts/07-analyze-change-request.prompt.md`. Then describe the change:

```
Change: in the Charging state the device must not be turned off by PowerManager.
Component: PowerManager
Related: ChargerManager (may provide the charging state)
```

### Claude Code: use the command

```bash
claude
/analyze-change-request
```

Then describe the change the same way.

### What the agent does

1. Reads the identified `.hpp` and `.cpp` files in full — no guessing from filenames
2. Reads the corresponding test files
3. Reads `main/main.cpp` to check if composition root wiring is affected
4. Produces a structured impact report:

```
Change Request: Block shutdown when ChargerManager reports Charging

Current behavior:
  PowerManager::shutdown() calls esp_restart() unconditionally
  (main/src/components/power_manager.cpp:47)

Required behavior:
  Return ShutdownResult::Blocked when ChargerManagerIf::getState() == ChargingState::Charging

Affected components:
  - PowerManager (main/src/components/power_manager.cpp)

Dependency blast radius:
  - PowerManagerIf interface: NOT changing (adding behavior, not signature)
  - ChargerManagerIf::getState() already exists — no interface change needed
  - Mocks: MockChargerManager in tests/host/power_manager_tests.cpp needs new return value

Test impact:
  FAIL (behavior changed):
    - test_shutdown_turns_off_device → must become conditional on charger state
  VALID (unchanged):
    - test_shutdown_after_timeout
    - test_shutdown_logs_reason
  NOT COMPILE: none

Composition root impact: none — ChargerManagerIf already injected into PowerManager

New files needed: none

Suggested pattern: Guard Pattern (check state at top of shutdown())

Acceptance criteria:
  - Given device is Charging, when PowerManager::shutdown() is called, then return Blocked
  - Given device is NotCharging, when PowerManager::shutdown() is called, then proceed normally
```

**Confirm the impact report before proceeding to Phase 8.**

---

## Phase 8: Add Feature / Implement Change Request

Implements the approved impact report from Phase 7. Follows the **minimal-change principle** — only the
files listed in the impact report are touched.

### Copilot

Attach `.github/copilot/prompts/08-add-feature.prompt.md`, paste the confirmed impact report:

```
[paste the confirmed Phase 7 impact report here]
Proceed with implementation.
```

### Claude Code

```bash
/add-feature
[paste the confirmed impact report]
```

### What the agent produces

1. **Modified source files** — only those listed in the impact report
2. **Updated test file** — stale tests updated with `// Updated: <reason>` comments, new test cases added
   for every acceptance criterion, no test cases deleted
3. **Build system update** — only if new `.cpp` files were genuinely added
4. **Summary table** — lists every file changed and why

### What it will NOT do

- Refactor surrounding code
- Rename or move files
- Touch components not in the impact report
- Delete test cases (uses `TEST_IGNORE_MESSAGE()` instead)

---

## Invoking Prompts in VS Code (Copilot)

Prompts are not auto-selected — you must attach them explicitly.

**Option 1** — paperclip icon in Copilot Chat → **Prompt...** → pick the file

**Option 2** — type `#` in the chat input, then start typing the prompt filename, press Tab to attach

**Option 3** — open the `.prompt.md` file in the editor; a **Run** button appears top-right

Note: `.github/copilot-instructions.md` IS loaded automatically on every session — you never need to attach
it manually. The architectural rules and skill-doc references are always active.

---

## Tips

### Providing Hardware Information

You don't need complete hardware details to start. The agent handles partial information:

- **Early in hardware development**: Describe peripherals in words. The agent will design interfaces that can
  be replaced when pin assignments are finalized.
- **Late in hardware development**: Provide a schematic image or netlist — Claude Code can parse image
  attachments.
- **Pin assignments can change**: They only affect the concrete implementation (`.cpp` files), not the
  interfaces or business logic.

### Iterating on Requirements

If the Phase 1 output doesn't match your intentions, provide specific corrections:
```
Correction: The sensor should also read every time a button is pressed (GPIO 0, active-low).
Add: HAL-004: ButtonHandlerIf — wraps gpio ISR — methods: registerPressCallback()
Add: FR-004: The device shall trigger an immediate sensor read when button is pressed
```

### When to Use Phase 4 vs Phase 7+8

- **Phase 3** (scaffold): Use only once, at the start, when you have an approved architecture
- **Phase 4** (generate-component): Adding a brand-new component with no impact on existing ones
- **Phases 7+8** (analyze + add-feature): Any change that touches existing components, interfaces, or tests

When in doubt, run Phase 7 first. The impact report is cheap — it costs one confirmation step but prevents
silently breaking existing behavior.

### Keeping the Skills Docs Current

If your team adopts a new convention or changes a standard, update the relevant file in `docs/skills/`. Both
Copilot and Claude will pick up the change automatically on the next prompt invocation.
