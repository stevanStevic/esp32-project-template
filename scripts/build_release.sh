#!/bin/bash
#
# Build and package ESP32 firmware for release or development.
#
# Usage:
#   ./scripts/build_release.sh --type dev                    # Dev build, version from git
#   ./scripts/build_release.sh --type release                # Release build (secure boot)
#   ./scripts/build_release.sh --type dev --name my-build    # Dev build with custom name
#
# Version / name resolution (in priority order):
#   1. --name <custom>        User-provided name
#   2. git tag on HEAD        If current commit is tagged, use the tag
#   3. git short SHA          Fallback
#
# Requirements:
#   - ESP-IDF environment must be sourced (IDF_PATH set)
#   - For release builds: signing key at keys/secure_boot_signing_key.pem
#     (download from S3: aws s3 cp s3://<bucket>/keys/secure_boot_signing_key.pem keys/)

set -e  # Exit immediately if a command fails
set -o pipefail  # Catch errors in piped commands

# Locate the ESP-IDF project directory (directory containing CMakeLists.txt + sdkconfig.defaults)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOP_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# Verify this is an ESP-IDF project
if [[ ! -f "$TOP_DIR/CMakeLists.txt" ]] || [[ ! -f "$TOP_DIR/sdkconfig.defaults" ]]; then
  echo "❌ Could not find ESP-IDF project at $TOP_DIR (missing CMakeLists.txt or sdkconfig.defaults)."
  exit 1
fi

echo "📌 Detected project root: $TOP_DIR"
cd "$TOP_DIR"

# Default values
BUILD_DIR="$TOP_DIR/build"
SIGNING_KEY="$TOP_DIR/keys/secure_boot_signing_key.pem"
RELEASE_TYPE="dev"  # Default to dev (safe default)
RELEASE_NAME=""     # Empty = auto-detect

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
    --type)
      RELEASE_TYPE="$2"
      shift 2
      ;;
    --name)
      RELEASE_NAME="$2"
      shift 2
      ;;
    *)
      echo "❌ Unknown argument: $1"
      exit 1
      ;;
  esac
done

# ── Resolve release name ────────────────────────────────────────────
if [[ -n "$RELEASE_NAME" ]]; then
  echo "📌 Using user-provided release name: $RELEASE_NAME"
elif TAG=$(git describe --tags --exact-match HEAD 2>/dev/null); then
  RELEASE_NAME="$TAG"
  echo "📌 Current commit is tagged: $RELEASE_NAME"
else
  RELEASE_NAME="$(git rev-parse --short HEAD)"
  echo "📌 No tag on HEAD, using commit SHA: $RELEASE_NAME"
fi

echo "🚀 Starting ${RELEASE_TYPE^^} Build..."
echo "📁 Using build directory: $BUILD_DIR"

# Ensure ESP-IDF is sourced
if [[ -z "$IDF_PATH" ]]; then
  echo "❌ ESP-IDF environment is not sourced. Please source export.sh before running this script."
  exit 1
fi

# Validate signing key for release builds
if [[ "$RELEASE_TYPE" == "release" ]]; then
  echo "🔑 Using signing key: $SIGNING_KEY"
  if [[ ! -f "$SIGNING_KEY" ]]; then
    echo "❌ Signing key not found: $SIGNING_KEY"
    echo "💡 Download it from S3:"
    echo "   aws s3 cp s3://<bucket>/keys/secure_boot_signing_key.pem keys/secure_boot_signing_key.pem"
    exit 1
  fi
fi

# Clean up previous build configuration
if [[ -f sdkconfig ]]; then
  echo "🧹 Removing old sdkconfig..."
  rm sdkconfig
fi

# Remove old build directory if it exists
if [[ -d "$BUILD_DIR" ]]; then
  echo "🗑️ Removing old build directory: $BUILD_DIR"
  rm -rf "$BUILD_DIR"
fi

# Create a new build directory
mkdir -p "$BUILD_DIR"

# Build the project based on release type
echo "🛠️ Building project..."
if [[ "$RELEASE_TYPE" == "release" ]]; then
  idf.py -B "$BUILD_DIR" -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.release" -D PROJECT_VER="$RELEASE_NAME" build || { echo "❌ Release build failed!"; exit 1; }
  echo "📦 Running release packaging script..."
  python3 "$TOP_DIR/scripts/create_release.py" --build-dir "$BUILD_DIR" --signing-key "$SIGNING_KEY" --name "$RELEASE_NAME" || { echo "❌ Release packaging failed!"; exit 1; }
elif [[ "$RELEASE_TYPE" == "dev" ]]; then
  idf.py -B "$BUILD_DIR" -D PROJECT_VER="$RELEASE_NAME" build || { echo "❌ Development build failed!"; exit 1; }
  echo "📦 Running development packaging script..."
  python3 "$TOP_DIR/scripts/create_release.py" --build-dir "$BUILD_DIR" --name "$RELEASE_NAME" || { echo "❌ Development packaging failed!"; exit 1; }
else
  echo "❌ Invalid release type: $RELEASE_TYPE. Use 'release' or 'dev'."
  exit 1
fi

echo "🎉 ${RELEASE_TYPE^^} build completed successfully!"
