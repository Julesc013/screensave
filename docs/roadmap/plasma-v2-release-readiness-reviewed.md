# Plasma v2 Release-Readiness Reviewed

Status: release-readiness-reviewed, not stable.

Recorded on: 2026-06-27.

Gate: `tools/scripts/check_plasma_v2_release_readiness.py`.

## Evidence

- Plasma v2 remains on the `plasma.v2.reference.preview` proof profile.
- Package staging lives under `packaging/windows/plasma-v2-preview/`.
- Manager preview can report pack provenance, license, installability refusal,
  proof refs, and rollback notes without real installation mutation.
- Workbench exposes release-readiness facts while continuing to consume the
  shared proof, pack, and evidence paths.
- The final artistic decision packet is accepted for release-candidate
  preparation only.
- AIDE WorkUnits, repair queue, and proposal-only agentic policy are active as
  development/evidence infrastructure.
- Fixed release-readiness capabilities are exposed without generic command,
  release, promotion, or agent-apply capabilities.

## Boundary

Release-readiness-reviewed is not stable release, release publication, final
stable artistic acceptance, compatibility certification, public SDK stability,
or all-saver migration.

The next track is `plasma-v2-release-candidate`.
