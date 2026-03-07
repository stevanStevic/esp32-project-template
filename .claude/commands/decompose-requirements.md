# Decompose Requirements

Break high-level business and hardware requirements into structured software requirements.

## Steps

1. Read `docs/skills/architecture-patterns.md`
2. Read `docs/skills/coding-standards.md`
3. Ask the user for (if not already provided):
   - High-level business purpose of the device
   - Hardware: ESP32 variant, peripherals, communication buses (SPI/I2C/UART/GPIO/CAN)
   - Known constraints (timing, memory, power, safety)
   - External interfaces the firmware exposes (WiFi REST, BLE GATT, UART commands, serial only, etc.)

## Output

Produce a structured requirements document:

**Functional Requirements (FR-xxx)**
`FR-001: The device shall ...`

**Non-Functional Requirements (NFR-xxx)**
`NFR-001: The system latency for X shall not exceed Y ms`

**Hardware Abstraction Requirements (HAL-xxx)**
For each peripheral: interface class name, methods, ESP-IDF APIs wrapped.
`HAL-001: GpioHandlerIf — wraps gpio_config/gpio_set_level/gpio_get_level — methods: setLevel(), getLevel(), registerIsr()`

**Inter-Task Communication Requirements (ITC-xxx)**
`ITC-001: cmdQueue: QueueIf<RequestCommand> from SpiManager to BusinessLogicManager`

**Component Identification**
List of business-logic components with responsibility, dependencies, consumers.

## Confirmation

End with:
> "Do these requirements match your intentions? Reply 'yes' to proceed to /design-architecture, or provide corrections."

Do NOT write any code files until the user confirms.
