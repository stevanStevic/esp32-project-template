#!/bin/bash

set -e  # Exit immediately if a command fails
set -o pipefail  # Catch errors in piped commands

# Locate the top-level ESP-IDF project directory
TOP_DIR=$(git rev-parse --show-toplevel 2>/dev/null || realpath "$(dirname "$(find . -name 'CMakeLists.txt' | head -n 1)")")

if [ -z "$TOP_DIR" ]; then
    echo "❌ Could not determine the top-level ESP-IDF project directory. Run this script inside an ESP-IDF project."
    exit 1
fi

echo "📌 Detected project root: $TOP_DIR"
cd "$TOP_DIR"

# Default values
BUILD_DIR="$TOP_DIR/build_release"
SIGNING_KEY="$TOP_DIR/keys/secure_boot_signing_key.pem"

# Parse script arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --signing-key)
            SIGNING_KEY="$2"
            shift 2
            ;;
        *)
            echo "❌ Unknown argument: $1"
            exit 1
            ;;
    esac
done

echo "🚀 Starting Release Build..."
echo "📁 Using build directory: $BUILD_DIR"
echo "🔑 Using signing key: $SIGNING_KEY"

# Ensure ESP-IDF is sourced
if [ -z "$IDF_PATH" ]; then
    echo "❌ ESP-IDF environment is not sourced. Please source export.sh before running this script."
    exit 1
fi

# Clean up previous build configuration
if [ -f sdkconfig ]; then
    echo "🧹 Removing old sdkconfig..."
    rm sdkconfig
fi

# Remove old build directory if it exists
echo $BUILD_DIR
if [ -d "$BUILD_DIR" ]; then
    echo "🗑️ Removing old build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Create a new build directory
mkdir -p "$BUILD_DIR"

# Build the project with the correct SDKCONFIG_DEFAULTS
echo "🛠️ Building project..."
idf.py -B "$BUILD_DIR" -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.release" build || { echo "❌ Build failed!"; exit 1; }

# Invoke the release packaging script
echo "📦 Running release packaging script..."
python3 "$TOP_DIR/scripts/create_release.py" --build-dir "$BUILD_DIR" --signing-key "$SIGNING_KEY" || { echo "❌ Release packaging failed!"; exit 1; }

echo "🎉 Release build completed successfully!"
