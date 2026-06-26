# Portable v2 native Windows current evidence

This directory records the Gate C native artifact evidence captured for the portable v2 seam.

Evidence source commit: `ff601685e9e540e2c6ad01107945adfe5783278c`

Captured on: 2026-06-26

## Summary

- `windows-current-x86` built 19/19 current `.scr` artifacts.
- `windows-current-tools` built 2/2 current companion tools.
- Manifest expected counts match observed counts for both profiles.
- PE audits report zero violations for both profiles.
- Nocturne v1 and ABI v1/v2 proof paths preserve the canonical RGBA hash.
- Ricochet v1 and ABI v1/v2 proof paths preserve frames 0, 4, 8, and 32 exactly.
- AIDE EvidencePackets are generated as development-plane references only.

## Receipt refs

- Saver build receipt: `out/gate-c/windows-current-x86/build-receipt.json`
- Saver manifest: `out/gate-c/windows-current-x86/windows_current_x86_scr.manifest.json`
- Saver PE audit: `out/gate-c/windows-current-x86/windows_current_x86_scr.pe-audit.json`
- Tool build receipt: `out/gate-c/windows-current-tools/build-receipt.json`
- Tool manifest: `out/gate-c/windows-current-tools/windows_current_tools.manifest.json`
- Tool PE audit: `out/gate-c/windows-current-tools/windows_current_tools.pe-audit.json`
- Proof refs: `out/gate-c/proofs/`
- AIDE evidence packet ref: `out/aide/evidence-bridge-check/ricochet.evidence-packet.json`

These files are evidence summaries and references. Binary build outputs remain out of repository history.

