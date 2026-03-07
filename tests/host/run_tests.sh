#!/bin/bash
# Run all host unit tests.
# Requires IDF_PATH to be set (source ESP-IDF export.sh or run inside devcontainer).

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

echo "🧪 Building host unit tests..."
cmake -B "${BUILD_DIR}" -S "${SCRIPT_DIR}" -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build "${BUILD_DIR}"

echo ""
echo "🧪 Running host unit tests..."
cd "${BUILD_DIR}"
ctest --output-on-failure

echo ""
echo "✅ All host unit tests passed."
