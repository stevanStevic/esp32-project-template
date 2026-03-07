# Setup CI

Finalise CI workflow and CODEOWNERS for this project.

## Steps

1. Read `docs/skills/security-and-release.md`

Ask the user:
1. Project name (for release ZIP and GitHub Release title)
2. Team GitHub usernames (for CODEOWNERS)
3. ESP-IDF version (default: v5.5.2)
4. Secure Boot + flash encryption needed?
5. S3 bucket name for signing key?

## Files to Create / Update

**`.github/CODEOWNERS`**
```
/main/ @<username>
/.github/ @<username>
/scripts/ @<username>
```

**`.github/workflows/esp32_release.yml`**
- Update project name in release title
- Verify ESP-IDF version
- Verify host test step
- Verify signing key handling

## GitHub Secrets

For release builds, instruct user to add:
- `SECURE_BOOT_SIGNING_KEY` (base64-encoded RSA-3072 key)

Provide the exact generation and encoding commands.
