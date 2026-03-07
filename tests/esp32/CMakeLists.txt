# Separate ESP-IDF project for on-target tests.
# Flash this project to the ESP32 to run hardware-dependent unit tests.
cmake_minimum_required(VERSION 3.5)

# Only build the components needed for the tests (no WiFi, BT, etc.)
idf_build_set_property(MINIMAL_BUILD ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Esp32Tests)
