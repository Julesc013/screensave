# ScreenSave AIDE Evidence Bridge

This bridge projects ScreenSave-owned proof outputs into a compact AIDE-facing
EvidencePacket JSON. It does not rerun image comparison, certify operating
system support, accept artistic quality, or promote a release.

Inputs:

- project-adapter receipt
- Proof Bundle v1 JSON
- adapter artifact manifest
- optional PE audit JSON

Output:

- `screensave.aide-evidence-packet.v0`

The ScreenSave proof bundle remains the domain payload. AIDE receives evidence
references and separated claim axes only.
