# Release Process

## Overview

This document outlines the step-by-step process to release production-ready firmware for the ESP-IDF project
using GitHub Actions CI/CD.

## Release Process Steps

### 1. Ensure `develop` is Release Ready

Before tagging a release, ensure that the `develop` branch contains the final tested code, ready for
production.

### 2. Tag the Release

Tag the `develop` branch with the desired version number:

```sh
# Example: Tagging version v1.0.0
git tag v1.0.0

git push origin v1.0.0
```

### 3. Generate Secure Boot Signing Key (Optional)

A new secure boot signing key can be generated for each release. If using an existing key, this step can be
skipped.

To generate a new signing key:

```sh
espsecure.py generate_signing_key --version 2 keys/secure_boot_signing_key.pem
```

### 4. Encode and Add the Signing Key to CI Secrets (Optional)

Before adding the signing key to GitHub Secrets, it must be base64 encoded:

```sh
base64 keys/secure_boot_signing_key.pem > encoded_signing_key.txt
```

Add the contents of `encoded_signing_key.txt` to GitHub Secrets under `SIGNING_KEY`.

### 5. Manually Trigger the CI Release Workflow

Run the GitHub Actions workflow manually with `test_mode=false`:

1. Go to **GitHub Repository** → **Actions**
2. Select the `Build and Release Firmware` workflow
3. Click **Run workflow**
4. Set `test_mode` to `false` to enable full release mode

### 6. GitHub Actions CI Job Execution

The CI job executes the following steps:

#### **Step 1: Checkout Repository**

Fetches the repository with full history and submodules.

```yaml
- name: Checkout repo
  uses: actions/checkout@v4
  with:
      fetch-depth: 0
      submodules: "recursive"
```

#### **Step 2: Determine Release Version**

Extracts the latest Git tag or falls back to a short commit hash.

```sh
TAG=$(git describe --tags --abbrev=0 2>/dev/null || echo "latest-$(git rev-parse --short HEAD)")
echo "release_version=$TAG" >> $GITHUB_OUTPUT
```

#### **Step 3: Decode and Save Signing Key**

Decodes the base64-encoded signing key stored in GitHub Secrets.

```sh
mkdir -p keys
SECURE_BOOT_SIGNING_KEY="$(pwd)/keys/secure_boot_signing_key.pem"
echo "${{ secrets.SIGNING_KEY }}" | base64 --decode > "$SECURE_BOOT_SIGNING_KEY"
```

#### **Step 4: Run Build Script**

Executes the build script using Espressif’s GitHub CI action.

```yaml
- name: Run build script
  uses: espressif/esp-idf-ci-action@v1.2.0
  with:
      esp_idf_version: v5.2.4
      target: esp32s3
      command: ./scripts/build_release.sh
```

#### **Step 5: Locate Built Firmware**

Finds the generated release ZIP file.

```sh
ARTIFACT_PATH=$(find release -type f -name "*.zip" | head -n 1)
if [[ -z "$ARTIFACT_PATH" ]]; then
  echo "Error: No release ZIP file found!"
  exit 1
fi
echo "artifact_path=$ARTIFACT_PATH" >> $GITHUB_OUTPUT
```

#### **Step 6: Upload Release to GitHub**

If `test_mode=false`, the firmware is uploaded as a GitHub release asset.

```yaml
- name: Upload release asset to GitHub Releases
  uses: softprops/action-gh-release@v2
  if: (github.event_name == 'push') || (github.event.inputs.test_mode != 'true')
  with:
      files: ${{ steps.find_release.outputs.artifact_path }}
      tag_name: ${{ steps.version.outputs.release_version }}
      name: "Project ${{ steps.version.outputs.release_version }}"
      body: "Automated firmware build for Project - Version ${{ steps.version.outputs.release_version }}."
      draft: false
      prerelease: false
  env:
      GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

## Scripts Breakdown

### **`build_release.sh` - Build Process**

1. Detects the project root and sets up the build directory.
2. Ensures ESP-IDF is sourced and SDK configuration files are cleaned up.
3. Builds the project with release-specific configurations.
4. Invokes the release packaging script.

```sh
idf.py -B "$BUILD_DIR" -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.release" build
```

### **`create_release.py` - Release Packaging**

1. Extracts project metadata from `project_description.json`.
2. Parses `flasher_args.json` to detect secure boot and encryption.
3. Modifies the flasher arguments for release consistency.
4. Generates a Secure Boot V2 public key digest (if applicable).
5. Creates a `flash.sh` script with safe flashing instructions.
6. Bundles the necessary files into a ZIP archive.

### **Secure Boot & Encryption Handling**

-   **Secure Boot Detection:** Ensures the bootloader is included in `flasher_args.json`.
-   **Encryption Handling:** Adds `--encrypt` flag to ensure proper encryption at flashing time.
-   **Flashing Script Generation:** Creates a `flash.sh` script that includes security warnings and necessary
    flashing steps.

```sh
echo "⚠️ Secure Boot is enabled! Use --force to flash the bootloader."
esptool.py write_flash --flash_mode dio --flash_freq 40m --flash_size detect --force 0x0 bootloader.bin
```

### **Release ZIP Package Contents**

The final output is a ZIP archive containing:

-   `flasher_args.json` – Modified flashing parameters
-   `flash.sh` – A script for flashing with security warnings
-   `digest.bin` – Secure Boot V2 public key digest (if applicable)
-   Firmware binary files

## Summary

The CI/CD pipeline automates the firmware build and release process, ensuring a smooth and secure release
workflow for ESP-IDF-based firmware. This approach ensures:

-   **Consistency:** Tagged releases are automatically built and packaged.
-   **Security:** Secure Boot and Encryption settings are enforced.
-   **Efficiency:** Eliminates manual intervention in firmware packaging and deployment.
