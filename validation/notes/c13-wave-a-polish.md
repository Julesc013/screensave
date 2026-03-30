# C13 Wave A Polish Note

`C13` stays the active saver-polish phase.
This note records the first implemented Wave A pass only:

- `nocturne`
- `ricochet`
- `deepfield`
- `plasma`
- `phosphor`

## Scope Locked In

This prompt stayed inside Wave A polish scope:

- no out-of-wave saver rewrites
- no new renderer tiers or renderer-capability expansion
- no packaging or installer expansion
- no suite-app or anthology feature expansion beyond existing validation surfaces

## Wave A Outcomes

| Product | Wave A result |
| --- | --- |
| `nocturne` | Stronger near-black curation, calmer long-run drift refresh, subtler ghost motion, and better museum/phosphor night coverage |
| `ricochet` | Cleaner deterministic starts, calmer long-run rhythm refresh, stronger trail taper, and more intentional amber/corporate variants |
| `deepfield` | Animated twinkle, calmer preview travel, periodic scene refresh, and clearer observatory vs travel preset separation |
| `plasma` | Stronger effect-family distinction, curated midnight and amber dark-room variants, and softer long-run composition refresh |
| `phosphor` | Better laboratory preset families, calmer long-run ratio refresh, preview-safe pacing, and stronger amber/white instrument variants |

## Shared Surface Changes

- Wave A preset import/export now accepts the human-facing product keys already used by the checked-in preset files across the in-wave savers.
- `deepfield` and `phosphor` now implement the shared preset/theme/randomization import-export hooks they already advertised through the shared saver contract.

## BenchLab Note

No Wave A BenchLab code changes were required.
The existing diagnostics surface already exposes renderer, preset, theme, randomization, and session-state information well enough to inspect the in-wave savers after this pass.

## Validation Level

Wave A validation in this environment is static and source-level:

- saver smoke tests were updated for the new presets, themes, and config-hook coverage
- Wave A surface validation is checked by `tools/scripts/check_wave_a_surface.py`
- no supported MSVC or MinGW toolchain was available here for a fresh native build or runtime capture pass

## Next Step

- `C13` Wave B has now landed for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`
- `C13` Wave C for the final saver-specific polish wave
