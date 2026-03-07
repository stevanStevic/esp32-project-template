"""
E2E test fixtures for pytest-embedded.

Provides a device-under-test (dut) fixture via serial connection using
pytest-embedded. Projects extend this with protocol-specific fixtures
(HTTP, BLE, UART commands, etc.) based on their external interfaces.

See docs/skills/testing-strategy.md for E2E patterns.
"""

import subprocess
import pytest


@pytest.fixture(scope="session", autouse=True)
def build_firmware():
    """
    Session-scoped fixture that builds the firmware before running E2E tests.
    Skipped if the build already exists (idf.py is smart enough not to rebuild
    if nothing changed).
    """
    import os

    project_root = os.path.join(os.path.dirname(__file__), "..", "..")
    result = subprocess.run(
        ["idf.py", "build"],
        cwd=project_root,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        pytest.fail(f"Firmware build failed:\n{result.stderr}")


# ---------------------------------------------------------------------------
# Project-specific fixtures
# ---------------------------------------------------------------------------
# Add your fixtures below based on the external interface your firmware exposes.
#
# Examples:
#
# @pytest.fixture
# def wifi_client(dut):
#     """Fixture for projects with a WiFi HTTP API."""
#     # Parse IP from serial, return requests.Session pointed at device
#     ...
#
# @pytest.fixture
# def ble_client(dut):
#     """Fixture for BLE projects using the bleak library."""
#     import asyncio
#     from bleak import BleakClient
#     ...
#
# @pytest.fixture
# def uart_dut(dut):
#     """Fixture for UART command/response projects."""
#     # dut.write_line() / dut.expect() are the primary tools
#     return dut
