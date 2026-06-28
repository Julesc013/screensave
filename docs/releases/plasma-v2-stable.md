# Plasma v2 Stable Publication-Prep Notes

Status: stable-promoted and publication-prep, not publicly published.

Plasma v2 is stable-promoted for `plasma-v2-rc1` in profile
`plasma.v2.reference.preview`. The final artistic verdict is
`accepted-for-stable`, the instrument architecture audit has no blockers, and
the stable-promotion gate is accepted for the bounded Plasma v2 rc1 scope.

## Included Scope

- Plasma v2 current release-candidate profile only.
- Direct-control instrument architecture.
- Software/reference path as canonical.
- GDI lower-band path.
- GL11 optional candidate.
- `packc` v1-candidate data packs.
- VisualIntent-to-spec candidate flow.
- Current Windows release-candidate evidence.

## Publication Packet

The local publication-prep packet is in
[`releases/plasma-v2-stable/`](../../releases/plasma-v2-stable/). It includes a
release manifest, artifact manifest, checksums, provenance, known limits,
support matrix, install notes, rollback notes, third-party notices, and a
publication checklist.

This packet references the existing stable-promotion stage and proof evidence.
It does not upload assets, create a GitHub release, publish a release page, or
verify public download links.

## Compatibility And Support Boundary

The support claim is evidence-classed and artifact-specific. Current Windows
x86 `.scr` evidence can support this Plasma v2 stable scope, but it does not
create broad Windows preservation certification. The software/reference path is
canonical, GDI remains mandatory, and GL11 remains optional.

No preservation certification broadening, macOS or Linux host claim, public SDK
stability, all Core saver migration, runtime executable plugin system, Manager
install/apply mutation, or graphical Workbench MVP is included in this packet.

## Evidence

- Final artistic decision:
  `validation/captures/plasma-v2/final-artistic-decision/decision.stable.toml`
- Stable-promotion gate:
  `validation/captures/plasma-v2/stable-promotion/gate-report.json`
- Instrument audit:
  `validation/captures/plasma-v2/instrument-audit/re-audit-report.json`
- Stable-promotion package:
  `packaging/windows/plasma-v2-stable-promotion/manifest.toml`
- Publication-prep manifest:
  `releases/plasma-v2-stable/release-manifest.toml`
