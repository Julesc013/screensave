# ScreenSave Installer Release Notes

This note is the user-facing release-support source for the frozen `C16` `ScreenSave Installer` companion package.

## What This Installer Is

- A scripted current-user installer package for the frozen ScreenSave Core payload
- A companion convenience layer, not the primary product
- A package that installs the current Core saver payload into a dedicated current-user install root and writes a current-user uninstall record
- The secondary asset on the public `c16-core-baseline` release page

## What This Installer Is Not

- Not a machine-wide installer
- Not the separate `suite` app
- Not BenchLab
- Not the authority that decides what belongs in Core

## Install And Registration Policy

- Supported install mode: current-user only
- Default install does not silently replace the current active saver
- Optional selection assistance is available through `install_screensave.ps1 -SetActiveSaver <slug>`
- Uninstall removes the dedicated install root and uninstall record while preserving user configuration outside that managed tree

## Payload Policy

- The Installer mirrors the frozen Core saver lineup, including `anthology`
- `suite`, BenchLab, SDK material, and Extras remain separate channels and are not installed by this package
- Reinstall overlays existing managed files and does not prune payload-absent savers automatically

## Evidence Snapshot

- `SS02` rehashed the published Installer ZIP and confirmed that its extracted payload still matches the frozen nineteen-saver Core lineup.
- `SS02` passed mock-mode current-user install and uninstall against the extracted Installer ZIP, including current-user selection assistance for `anthology`.
- `SS02` did not turn mock-mode evidence into a live registry or Control Panel claim.

## Current Known Limits

- Machine-wide install, shell-wide registration helpers, and richer upgrade repair modes remain deferred
- Fresh live installer registry smoke is still not claimed here beyond the packaged current-user policy, staged payload evidence, and mock-mode validation
