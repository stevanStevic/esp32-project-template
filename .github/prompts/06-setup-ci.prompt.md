---
mode: agent
description: Finalise CI workflow and CODEOWNERS for the project.
---

# Phase 6: CI Setup

You are an expert DevOps engineer for ESP-IDF projects. Finalize the CI/CD configuration for this project.

## Instructions

Read before starting:
- `docs/skills/security-and-release.md`

Ask the user:
1. What is the project name? (used in release ZIP filename and GitHub Release title)
2. Who are the team members? (GitHub usernames for CODEOWNERS)
3. Which ESP-IDF version? (default: v5.5.2)
4. Is Secure Boot / flash encryption needed for releases?
5. Is there an S3 bucket for signing key storage?

## Files to Create / Update

### `.github/CODEOWNERS`
```
# Auto-generated CODEOWNERS

# All source code
/main/ @<github-username>

# CI/scripts
/.github/ @<github-username>
/scripts/ @<github-username>
```

### `.github/workflows/esp32_release.yml`
Update:
- Project name in release title and artifact name
- ESP-IDF version if different from v5.5.2
- Verify host test step references `tests/host/run_tests.sh`
- Verify signing key step is correct for the chosen key storage

### Verify Scripts Work
Check that:
- `scripts/build_release.sh` runs without error for a dev build
- `scripts/run_all_tests.sh --host` runs and exits 0 (or reports no tests if none exist yet)
- `scripts/run_unit_tests_host.sh` is executable

## GitHub Secrets Required

For release builds with Secure Boot, instruct the user to add:
- `SECURE_BOOT_SIGNING_KEY` — base64-encoded RSA-3072 private key

Provide the exact command to generate and encode the key.
