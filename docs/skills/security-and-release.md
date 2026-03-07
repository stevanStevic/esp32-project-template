# Security and Release

Reference for secure boot, flash encryption, signing key management, and the release process.

---

## Build Types

| Type | Secure Boot | Flash Encryption | Used for |
|---|---|---|---|
| `dev` | No | No | Development and testing |
| `release` | Yes (V2/RSA-3072) | Yes (AES-128) | Production devices |

---

## Secure Boot V2

Configured in `sdkconfig.release`:
```
CONFIG_SECURE_BOOT_V2_RSA_ENABLED=y
CONFIG_SECURE_BOOT_SIGNING_KEY="keys/secure_boot_signing_key.pem"
```

The signing key is used at **build time** to sign the bootloader and application binaries. The private key is
**never** shipped with the firmware — only `digest.bin` (public key hash) is included in the release ZIP.

---

## Flash Encryption

Configured in `sdkconfig.release`:
```
CONFIG_SECURE_FLASH_ENC_ENABLED=y
CONFIG_SECURE_FLASH_ENCRYPTION_AES128=y
CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE=y
```

Once flash encryption is enabled on a device, all future firmware must be flashed with the `--encrypt` flag.
**Do not lose the encryption key** — encrypted devices cannot be recovered without it.

---

## Signing Key Management

| Location | Purpose |
|---|---|
| **S3 bucket** | Source of truth — never deleted |
| **GitHub Secret** (`SECURE_BOOT_SIGNING_KEY`) | Base64-encoded for CI |
| **Local** (`keys/secure_boot_signing_key.pem`) | Downloaded from S3, gitignored |

The `keys/` directory is gitignored. Never commit signing keys.

### Generating a New Key

```bash
espsecure.py generate_signing_key --version 2 keys/secure_boot_signing_key.pem

# Upload to S3
aws s3 cp keys/secure_boot_signing_key.pem s3://<bucket>/keys/secure_boot_signing_key.pem --sse AES256

# Encode for GitHub Secret
base64 -w 0 keys/secure_boot_signing_key.pem
# Paste into: GitHub repo → Settings → Secrets → SECURE_BOOT_SIGNING_KEY
```

### Downloading Key for Local Release Build

```bash
aws s3 cp s3://<bucket>/keys/secure_boot_signing_key.pem keys/secure_boot_signing_key.pem
chmod 600 keys/secure_boot_signing_key.pem
```

---

## Local Build

```bash
# Dev build (no signing key needed)
./scripts/build_release.sh --type dev

# Release build (signing key required)
./scripts/build_release.sh --type release

# Custom name
./scripts/build_release.sh --type dev --name v1.2.3-beta
```

Output ZIP is placed in `release/`.

---

## CI Build (GitHub Actions)

Workflow: `.github/workflows/esp32_release.yml` — triggered manually from the Actions tab.

Steps:
1. Checkout repository
2. Determine release name (user input → git tag → commit SHA)
3. Restore signing key from GitHub Secret (release builds only)
4. Run host tests (`tests/host/run_tests.sh`)
5. Build and package firmware (`build_release.sh`)
6. Upload artifact (always — downloadable from Actions tab)
7. Upload to GitHub Releases (optional checkbox)

---

## Release ZIP Contents

| File | Dev | Release |
|---|---|---|
| `flasher_args.json` | Yes | Yes |
| `flash.sh` | Yes | Yes (with secure boot/encryption warnings) |
| `digest.bin` | No | Yes |
| `bootloader/bootloader.bin` | Yes | Yes (signed) |
| `partition_table/partition-table.bin` | Yes | Yes |
| `<ProjectName>.bin` | Yes | Yes (signed) |
| `ota_data_initial.bin` | Yes | Yes |

---

## Semantic Versioning

Tags follow `vMAJOR.MINOR.PATCH` (e.g., `v1.0.0`, `v1.2.3`). Use annotated tags:

```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

The build version (`PROJECT_VER`) is passed to `idf.py` and embedded in the firmware binary, accessible at
runtime via `esp_app_get_description()->version`.
