# Plasma U08 Surface Evaluation

## Purpose

This note records the actual `U08` evaluation of Plasma's surviving surfaces
against the formal QA bar.

It is the evidence-binding layer between the earlier proof substrate and the
later `U09` recut.

## Evidence Sources Consumed

`U08` consumes the current repo-grounded evidence set:

- `U03` surface and control inventory notes
- `U04` output, treatment, and presentation truth notes
- `U05` simplification decisions and post-cut invariants
- `U06` retuning notes and default-path proof
- `U07` settings-influence proof, stable combination matrix, experimental
  coverage report, and diff or duplicate audit
- current `plasma_lab.py` reports for control profiles, curated collections,
  and the new `qa-bar`, `surface-evaluation`, and `acceptance-decisions`
  commands

## Surfaces Evaluated

### Stable Candidates Evaluated

- Basic dialog surface
- default path: `plasma_lava` plus `plasma_lava` on `gdi` and `gl11`
- stable preset pool: `classic_core`
- stable theme pool: `classic_core`
- stable curated collections:
  - `classic_core_featured`
  - `dark_room_support`
- stable support profiles:
  - `deterministic_classic`
  - `dark_room_support`
- stable grammar surfaces:
  - `raster` / `native_raster`
  - `flat`

### Experimental Candidates Evaluated

- Advanced dialog surface
- Author/Lab dialog surface
- banded and contour output families
- glyph output families
- direct filter, emulation, and accent controls
- `heightfield` and `ribbon` presentation
- transitions, journeys, and timing controls
- `wave3_experimental_sampler`

### Compatibility And Support-Only Surfaces Evaluated

- hidden `favorites_only` compatibility state
- compatibility-only preset and theme aliases
- diagnostics overlay

## Evaluation Results

| Surface | Result | Reason |
| --- | --- | --- |
| Basic dialog surface | `pass_stable` | The shipped Basic controls now have stable influence proof and stay aligned with the lower-band-safe default story. |
| Default path `plasma_lava` plus `plasma_lava` | `pass_stable` | This remains the strongest default evidence path across `gdi` and `gl11` and preserves the historical center of gravity honestly. |
| `classic_core` preset set | `pass_stable` | The stable preset pool is small enough to prove, retuned enough to keep, and still preserves Classic identity. |
| `classic_core` theme set | `pass_stable` | The stable theme pool remains supportable, with `aurora_cool` versus `oceanic_blue` kept on a watch list rather than promoted beyond the evidence. |
| `classic_core_featured` collection | `pass_stable` | It remains stable-worthy once the implied stable journey claim is removed. |
| `dark_room_support` collection | `pass_stable` | It remains a calmer stable slice once the implied stable journey claim is removed. |
| `deterministic_classic` profile | `pass_stable` | The deterministic support profile remains stable-worthy after removing the journey implication. |
| `dark_room_support` profile | `pass_stable` | The dark-room support profile remains stable-worthy after removing the journey implication. |
| `raster` / `native_raster` | `pass_stable` | Raster remains the truthful lower-band default and the only stable output-family claim. |
| `flat` presentation | `pass_stable` | Flat remains the default, lower-band-honest presentation path. |
| Advanced dialog surface | `pass_experimental` | Real controls, real influence, but broader than the stable product promise. |
| banded and contour output | `pass_experimental` | Materially real and distinct enough to keep exposed, but still not part of the narrow stable promise. |
| direct treatment controls | `pass_experimental` | The direct treatment knobs materially alter the admitted subset, but are still broader than stable. |
| `wave3_experimental_sampler` | `pass_experimental` | The sampler remains a useful bounded experimental gallery. |
| Author/Lab dialog surface | `keep_experimental_with_caveat` | Several controls remain plan-heavy or support-facing and should stay bounded. |
| glyph output | `keep_experimental_with_caveat` | Real enough to keep, but still bounded to a narrow ASCII and Matrix subset. |
| `heightfield` and `ribbon` | `keep_experimental_with_caveat` | Real presentation paths, but still premium-dependent and lower-lane-caveated. |
| transitions, journeys, and timing | `keep_experimental_with_caveat` | Implemented and useful, but still only partially proved and therefore not stable-worthy. |
| hidden `favorites_only` state | `remove_from_claim_surface` | Preserved only for compatibility semantics, not as first-class variety. |
| compatibility-only preset and theme aliases | `remove_from_claim_surface` | Migration and identity surfaces remain preserved without claiming them as current product breadth. |
| diagnostics overlay | `demote_hide` | Useful for support and proof, but not for the shipped product claim surface. |

## Baseline And Default-Path Evaluation

The baseline/default path remains acceptable after `U08`:

- `default_preset=plasma_lava`
- `default_theme=plasma_lava`
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

`U08` does not redefine that baseline.
It explicitly judges it and keeps it.

## Blocked And Partial Areas

No major surviving surface needed a `blocked_insufficient_evidence` verdict in
this tranche.

The most important partial areas remain:

- transitions, journeys, and timing controls
- deterministic seed replay identity as a visual surface rather than only a
  structural guarantee
- premium presentation surfaces beyond the bounded current subset

Those surfaces stay bounded and caveated rather than silently promoted.
