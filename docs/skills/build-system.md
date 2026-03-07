# Build System

Reference for ESP-IDF CMake patterns and build configuration used in this template.

---

## Component Registration

Every source file must be listed in `main/CMakeLists.txt` via `idf_component_register()`:

```cmake
idf_component_register(
    SRCS
        "main.cpp"
        "src/app.cpp"
        "src/components/gpio_handler.cpp"
        "src/components/spi_manager.cpp"
        "src/components/logger.cpp"
    INCLUDE_DIRS
        "include")
```

When adding a new component:
1. Create `main/include/component_interfaces/foo_if.hpp`
2. Create `main/include/components/foo.hpp`
3. Create `main/src/components/foo.cpp`
4. Add `"src/components/foo.cpp"` to SRCS in `main/CMakeLists.txt`

---

## Managed Component Dependencies

External library dependencies declared in `main/idf_component.yml`:

```yaml
dependencies:
  idf: ">=5.3.0"
  espressif/button: "*"
  bblanchon/arduinojson: "^7.4.2"
```

Install/update with:
```bash
idf.py reconfigure   # Downloads managed components into managed_components/
```

The `managed_components/` directory is gitignored — always run `idf.py reconfigure` after cloning.

---

## Kconfig Project Configuration

`main/Kconfig.projbuild` defines runtime-configurable options visible in menuconfig:

```kconfig
menu "Project Configuration"

    config HARDWARE_PLATFORM_NAME
        string "Hardware platform name"
        default "ESP32-S3"
        help
            Identifies the hardware platform in device info responses.

    config MY_FEATURE_ENABLED
        bool "Enable My Feature"
        default y
        help
            Set to n to compile out the feature entirely.

endmenu
```

Access in code: `CONFIG_HARDWARE_PLATFORM_NAME`, `CONFIG_MY_FEATURE_ENABLED`

---

## Partition Table

The default partition table is `custom_partitions.csv` — a 16MB OTA layout:

```
nvs,      data, nvs,     ,        24K,
otadata,  data, ota,     ,        8K,
phy_init, data, phy,     ,        4K,
ota_0,    app,  ota_0,   0x20000, 4M,
ota_1,    app,  ota_1,   ,        4M,
storage,  data, spiffs,  ,        7M,
coredump, data, coredump,,        64K,
```

Configured in `sdkconfig.defaults`:
```
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="custom_partitions.csv"
```

---

## sdkconfig Layering

Two config layers are merged at build time:

| File | Purpose |
|---|---|
| `sdkconfig.defaults` | Base config: flash size, PSRAM, partition table, core dump |
| `sdkconfig.release` | Release overlay: optimization, secure boot, flash encryption, watchdogs |

Dev build (default): only `sdkconfig.defaults` is applied.
Release build: `sdkconfig.defaults` + `sdkconfig.release` merged.

This is how `build_release.sh` controls it:
```bash
# Dev
idf.py -D PROJECT_VER="..." build

# Release
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.release" -D PROJECT_VER="..." build
```

The generated `sdkconfig` is gitignored. Never commit it.

---

## Building the Project

```bash
idf.py build                     # Dev build
idf.py -p /dev/ttyACM0 flash    # Flash to device
idf.py -p /dev/ttyACM0 monitor  # Serial monitor
idf.py fullclean                 # Clean everything
```

Via VS Code: use the ESP-IDF extension tasks defined in `.vscode/tasks.json`.

---

## Host Test Build System

Host tests use a standalone CMake project (not ESP-IDF):

```cmake
cmake_minimum_required(VERSION 3.16)
project(HostTests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Unity from ESP-IDF
set(UNITY_DIR $ENV{IDF_PATH}/components/unity/unity/src)

# Each test gets its own executable
add_executable(foo_tests
    foo_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../main/src/components/foo.cpp
    utils/src/logger_mock.cpp)

target_include_directories(foo_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../main/include
    utils/include
    ${UNITY_DIR})
```

See `tests/host/CMakeLists.txt` for the complete template.
