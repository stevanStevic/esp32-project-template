# Release Process

## Overview

This document outlines how to build and release ESP32 firmware, both locally and via GitHub Actions CI/CD. Two
build types are supported:

- **Dev build** - No secure boot, no flash encryption. For development and testing.
- **Release build** - Secure Boot V2 + flash encryption enabled. For production devices.

Both local and CI builds use the same `build_release.sh` script, producing identical output.

---

## Local Build

### Prerequisites

- ESP-IDF environment sourced (`source $IDF_PATH/export.sh`)
- For release builds: signing key at `keys/secure_boot_signing_key.pem`

### Dev Build (No Secure Boot)

```sh
cd code/esp32
./scripts/build_release.sh --type dev
```

This uses `sdkconfig.defaults` only. Output ZIP is in `release/`.

### Release Build (Secure Boot + Encryption)

First, obtain the signing key from S3:

```sh
# One-time: configure AWS SSO access
aws configure sso

# Download signing key
aws s3 cp s3://<S3-storage-path>/keys/secure_boot_signing_key.pem keys/secure_boot_signing_key.pem
chmod 600 keys/secure_boot_signing_key.pem
```

Then build:

```sh
cd code/esp32
./scripts/build_release.sh --type release
```

This uses `sdkconfig.defaults` + `sdkconfig.release` overlay (enables Secure Boot V2, flash encryption).
Output ZIP is in `release/`.

> **Note:** The signing key is stored on S3 and is **never committed to the repo**. The `keys/` directory is
> gitignored.

### Comparing Local vs CI Output

Both produce the same ZIP structure. To verify:

```sh
unzip -l release/AppEsp32_*.zip
```

Expected contents:

| File                                  | Dev build | Release build                                           |
| ------------------------------------- | --------- | ------------------------------------------------------- |
| `flasher_args.json`                   | Yes       | Yes (+ `--force`, `--encrypt` flags, security metadata) |
| `flash.sh`                            | Yes       | Yes (+ secure boot and encryption warnings)             |
| `digest.bin`                          | No        | Yes (Secure Boot V2 public key digest)                  |
| `bootloader/bootloader.bin`           | Yes       | Yes (signed)                                            |
| `partition_table/partition-table.bin` | Yes       | Yes                                                     |
| `AppMain.bin`                         | Yes       | Yes (signed)                                            |
| `ota_data_initial.bin`                | Yes       | Yes                                                     |

---

## CI Build (GitHub Actions)

### Workflow: "ESP32 Release Build"

Located at `.github/workflows/esp32-release.yml`. Triggered manually from the Actions tab.

### Inputs

| Input                     | Type                | Description                                             |
| ------------------------- | ------------------- | ------------------------------------------------------- |
| **Branch/Tag**            | Dropdown (built-in) | Select which branch or tag to build from                |
| **Release name**          | Text (optional)     | Custom release name. Falls back to: tag then commit SHA |
| **Build type**            | Dropdown            | `dev` (default) or `release`                            |
| **Upload to GH Releases** | Checkbox            | If checked, attaches ZIP to a GitHub Release            |

### Creating and Pushing a Tag

Before triggering a release build from a tag, you need to create and push it:

```sh
# Ensure you're on the correct branch and up to date
git checkout develop
git pull

# Create an annotated tag
git tag -a v1.0.0 -m "Release v1.0.0"

# Push the tag to the remote
git push origin v1.0.0
```

> **Tip:** Use [semantic versioning](https://semver.org/) for tags (e.g., `v1.0.0`, `v1.1.0`, `v2.0.0`).
> Annotated tags (`-a`) are preferred over lightweight tags as they include author, date, and message
> metadata.

To list existing tags:

```sh
git tag -l "v*"
```

To delete a tag if created by mistake:

```sh
# Delete locally
git tag -d v1.0.0

# Delete from remote
git push origin --delete v1.0.0
```

### How to Run

1. Go to **GitHub Repository** → **Actions**
2. Select **ESP32 Release Build**
3. Click **Run workflow**
4. Select a **tag** (e.g., `v1.0.0`) or branch from the dropdown
5. Optionally enter a **release name**
6. Select **build type** (`dev` or `release`)
7. Check **Upload to GH Releases** if you want to publish

### CI Steps

1. **Checkout** - Fetches repo at selected ref with full history
2. **Determine release name** - User input → git tag → commit SHA fallback
3. **Restore signing key** - (release builds only) Decodes `SECURE_BOOT_SIGNING_KEY` GitHub Secret
4. **Run host tests** - Runs SPI library and ESP32 host tests inside ESP-IDF v5.5.2 Docker container
5. **Build and package** - Runs `./scripts/build_release.sh --type <dev|release>` inside ESP-IDF v5.5.2 Docker
   container
6. **Upload artifact** - ZIP always available as a workflow artifact (downloadable from Actions tab)
7. **Upload to GitHub Releases** - (if checkbox checked) Creates a GitHub Release with ZIP attached

### CI Secrets

For release builds, the following secret must be configured:

| Secret                    | Description                         |
| ------------------------- | ----------------------------------- |
| `SECURE_BOOT_SIGNING_KEY` | Base64-encoded RSA-3072 signing key |

To set it up (one-time):

```sh
base64 -w 0 keys/secure_boot_signing_key.pem
# Copy output → GitHub repo Settings → Secrets → New secret → SECURE_BOOT_SIGNING_KEY
```

---

## Signing Key Management

| Location                                       | Purpose                             |
| ---------------------------------------------- | ----------------------------------- |
| **S3** (`s3://<S3-storage-path>/keys/`)        | Source of truth for the signing key |
| **GitHub Secret** (`SECURE_BOOT_SIGNING_KEY`)  | Base64-encoded copy for CI          |
| **Local** (`keys/secure_boot_signing_key.pem`) | Downloaded from S3, gitignored      |

The private key is used at **build time only** to:

1. Sign the bootloader and app binaries (done by `idf.py build` with `sdkconfig.release`)
2. Derive `digest.bin` (public key hash, done by `create_release.py`)

The private key is **never** included in the release ZIP. Only `digest.bin` (public key hash) ships.

### Generating a New Key

```sh
espsecure.py generate_signing_key --version 2 keys/secure_boot_signing_key.pem

# Upload to S3
aws s3 cp keys/secure_boot_signing_key.pem s3://<S3-storage-path>/keys/secure_boot_signing_key.pem --sse AES256

# Base64 encode for GitHub Secret
base64 -w 0 keys/secure_boot_signing_key.pem
# Paste output into GitHub Secrets as SECURE_BOOT_SIGNING_KEY
```

---

## Scripts

### `build_release.sh`

Unified build script for local and CI use.

```sh
./scripts/build_release.sh --type dev       # Dev build
./scripts/build_release.sh --type release   # Release build (needs signing key)
```

Steps:

1. Detects ESP-IDF project root
2. Validates signing key (release builds only)
3. Cleans old `sdkconfig` and build directory
4. Builds with appropriate config overlay
5. Calls `create_release.py` to package the release ZIP

### `create_release.py`

Packages build output into a release ZIP.

1. Parses `flasher_args.json` - detects secure boot and encryption settings
2. Modifies flash arguments for release (adds `--force`, `--encrypt` flags if needed)
3. Generates `digest.bin` from signing key (if secure boot detected)
4. Creates `flash.sh` script with appropriate warnings
5. Bundles everything into a ZIP archive

### Release ZIP Contents

| File                                  | Description                                            |
| ------------------------------------- | ------------------------------------------------------ |
| `flasher_args.json`                   | Modified flash addresses and settings                  |
| `flash.sh`                            | One-command flash script with security warnings        |
| `digest.bin`                          | Secure Boot V2 public key digest (release builds only) |
| `bootloader/bootloader.bin`           | Second-stage bootloader (signed in release builds)     |
| `partition_table/partition-table.bin` | Compiled partition table                               |
| `MainApp.bin`                         | Main application (signed in release builds)            |
| `ota_data_initial.bin`                | OTA data partition initial state                       |
