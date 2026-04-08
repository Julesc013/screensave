# PL13 Known Limits

## Purpose

This document records the current repo-grounded limits, gaps, and caveats for Plasma at the end of `PL13`.
It is factual, current-state documentation, not a wishlist.

## Unsupported Features Or Paths

- Only `heightfield` is implemented for dimensional presentation.
- Other premium-dimensional presentation candidates such as curtain, ribbon, contour extrusion, bounded surface, and billboard-style presentation remain unsupported.
- The repo still does not claim scenic, world-simulated, or free-camera premium behavior.

## Partially Validated Features Or Paths

- Transition proof is bounded to the curated implemented subset, not every possible preset or theme pairing.
- BenchLab forcing is bounded to the implemented Plasma-specific forcing subset, not every imaginable future knob.
- Performance envelopes are bounded qualitatively rather than with hard numeric SLAs.

## Known Degrade And Fallback Caveats

- Premium presentation requests can degrade honestly back to the compat baseline when the requested path is not available, but PL13 does not claim every theoretical multi-hop forcing combination was exercised live.
- Transition fallback coverage is grounded in the implemented fallback policies and curated pairs already present in the product, not in speculative future content families.
- Lower-band truth remains the reference floor; richer-lane requests must still be treated as optional uplifts.

## Known Content, Transition, Settings, And BenchLab Limits

- The current repo ships stable-only Plasma content, so experimental filtering exists structurally but has no shipped experimental pool to validate against as content.
- Settings surfaces are real and validated for the shipped subset, but PL13 does not claim a complete future catalog of every eventual product control.
- BenchLab reporting and forcing are real for the implemented subset, but PL13 does not claim a full product debugger or unrestricted forcing console.

## Test-Environment Gaps

- The current capture set comes from one capable validation machine.
- PL13 does not include a wider cross-driver, cross-GPU, or low-capability denial sweep.
- Fullscreen `.scr` host behavior is not being claimed from BenchLab captures alone.

## Risks And Blockers Relevant To PL14

- Release-cut decisions will need to respect that cross-hardware coverage is still blocked.
- Release messaging must stay within the validated premium subset and must not imply more than `heightfield` for dimensional presentation.
- Transition claims must stay bounded to the implemented supported subset unless PL14 gathers more pairwise proof.
- Numeric performance promises remain out of bounds until a later measurement pass exists.

## Grounded Limit Register

| Limit Key | Status | Meaning |
| --- | --- | --- |
| `experimental_pool_empty` | Documented Only | Experimental filtering exists, but no shipped experimental Plasma content exists in the current repo. |
| `premium_heightfield_only` | Unsupported | `heightfield` is the only implemented dimensional presentation mode. |
| `transition_pair_coverage_bounded` | Partial | Transition proof covers the shipped supported subset, not every theoretical pair. |
| `benchlab_forcing_bounded` | Partial | BenchLab forcing covers the implemented subset only. |
| `cross_hardware_gap` | Blocked | Proof comes from one capable validation machine rather than a wider hardware sweep. |
| `numeric_perf_sla_gap` | Blocked | PL13 does not claim hard numeric frame-time, memory, or startup SLAs. |

## Scope Boundary

These limits are current-repo-grounded facts at the end of `PL13`.
They do not predict or promise what later work will necessarily remove.
