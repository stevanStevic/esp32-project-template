cmake_minimum_required(VERSION 3.16)
project(HostTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g -O0 -Wall -Wextra)
enable_testing()

# ── Unity test framework (from ESP-IDF) ───────────────────────────────────────
# Requires IDF_PATH environment variable to be set.
# Inside devcontainer or after sourcing export.sh, IDF_PATH is always set.
if(NOT DEFINED ENV{IDF_PATH})
    message(FATAL_ERROR "IDF_PATH is not set. Source ESP-IDF export.sh first.")
endif()

set(UNITY_DIR "$ENV{IDF_PATH}/components/unity/unity/src")
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/../..")

# Build Unity as a static library
add_library(unity_lib STATIC ${UNITY_DIR}/unity.c)
target_include_directories(unity_lib PUBLIC ${UNITY_DIR})

# ── Shared test utilities ─────────────────────────────────────────────────────
set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils/include")
set(UTILS_SRC_LOGGER  "${CMAKE_SOURCE_DIR}/utils/src/logger_mock.cpp")

# ── Test targets ──────────────────────────────────────────────────────────────
# Add one block per component under test.
#
# Pattern:
#   add_executable(<component>_tests
#       <component>_tests.cpp
#       ${PROJECT_ROOT}/main/src/components/<component>.cpp
#       ${UTILS_SRC_LOGGER})
#
#   target_include_directories(<component>_tests PRIVATE
#       ${PROJECT_ROOT}/main/include
#       ${UTILS_INCLUDE_DIR}
#       ${UNITY_DIR})
#
#   target_link_libraries(<component>_tests unity_lib)
#   add_test(NAME <component>_tests COMMAND <component>_tests)
#
# TODO: Add test targets here as you add components. Example:
#
# add_executable(my_component_tests
#     my_component_tests.cpp
#     ${PROJECT_ROOT}/main/src/components/my_component.cpp
#     ${UTILS_SRC_LOGGER})
#
# target_include_directories(my_component_tests PRIVATE
#     ${PROJECT_ROOT}/main/include
#     ${UTILS_INCLUDE_DIR}
#     ${UNITY_DIR})
#
# target_link_libraries(my_component_tests unity_lib)
# add_test(NAME my_component_tests COMMAND my_component_tests)
