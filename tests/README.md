# Tests

This project uses a 3-tier testing strategy. See `docs/skills/testing-strategy.md` for full details.

## Tier 1: Host Unit Tests (`tests/host/`)

Run on any x86 Linux machine. No ESP32 hardware or ESP-IDF toolchain needed (only `IDF_PATH` for Unity
headers).

```bash
./tests/host/run_tests.sh
```

## Tier 2: On-Target Tests (`tests/esp32/`)

Require an ESP32-S3 connected via USB.

```bash
./tests/esp32/run_tests.sh
```

## Tier 3: E2E / Integration Tests (`tests/e2e/`)

Require a flashed ESP32 and the full hardware setup. The verification approach depends on the project's
external interfaces (serial output, HTTP, BLE, UART, etc.).

```bash
cd tests/e2e && pytest --target esp32s3
```

## Run All

```bash
./scripts/run_all_tests.sh --all      # All tiers
./scripts/run_all_tests.sh --host     # Host only (CI-safe, no hardware)
./scripts/run_all_tests.sh --esp32    # On-target only
./scripts/run_all_tests.sh --e2e      # E2E only
```
