# PL13 Known Limits

## Purpose

This document records the current repo-grounded limits, gaps, and caveats for Plasma at the end of `PL13`.
It is factual, current-state documentation, not a wishlist.

## Unsupported Features Or Paths

- Only `heightfield` is implemented for dimensional presentation.
- Other premium-dimensional presentation candidates such as curtain, ribbon, contour extrusion, bounded surface, and billboard-style presentation remain unsupported.
- The repo still does not claim scenic, world-simulated, or free-camera premium behavior.

## Partially Validated Features Or Paths

- Transition proof is bounded to the curated implemented subset, named compatibility classes, and curated bridge-cycle journeys rather than every possible preset or theme pairing.
- The PX30 experimental content pool is real but bounded to a small preset slice rather than a broad experimental catalog.
- Contour and banded output are now real for a bounded subset rather than taxonomy only, but non-raster breadth remains partial.
- Treatment-family support is now real for a curated subset rather than `none` only, but the broader treatment taxonomy remains intentionally partial.
- BenchLab forcing is bounded to the implemented Plasma-specific forcing subset, not every imaginable future knob.
- Performance envelopes are bounded qualitatively rather than with hard numeric SLAs.
- Multi-monitor support is bounded to the shared one-window virtual-desktop baseline rather than independent per-monitor Plasma behavior.
- Environment safety support is bounded to current preview-safe, long-run-stable, descriptive dark-room content, and truthful clamp or fallback behavior.
- The authored substrate is still partial: sets, journeys, and pack provenance are on disk, but built-in preset and theme descriptors are still primarily compiled and legacy-INI anchored.
- The first Plasma Lab shell is CLI-only and support-oriented rather than a full editor.
- Selection intelligence is still bounded to weighted authored fallback, existing favorites/exclusions, and current set or journey hooks.

## Known Degrade And Fallback Caveats

- Premium presentation requests can degrade honestly back to the compat baseline when the requested path is not available, but PL13 does not claim every theoretical multi-hop forcing combination was exercised live.
- Transition fallback and hard-cut coverage are grounded in the implemented fallback policies, named compatibility classes, and curated bridge-cycle pairs already present in the product, not in speculative future content families.
- Lower-band truth remains the reference floor; richer-lane requests must still be treated as optional uplifts.

## Known Content, Transition, Settings, And BenchLab Limits

- The current repo now ships a bounded experimental preset slice for PX30 output and treatment studies, but the default pool remains stable-only and the broader experimental catalog is still absent.
- Settings surfaces are real and validated for the shipped subset, but PL13 does not claim a complete future catalog of every eventual product control.
- BenchLab reporting and forcing are real for the implemented subset, but PL13 does not claim a full product debugger or unrestricted forcing console.

## Test-Environment Gaps

- The current capture set comes from one capable validation machine.
- PL13 does not include a wider cross-driver, cross-GPU, or low-capability denial sweep.
- Mixed-topology and Plasma-specific preview-host reruns are not part of the current checked-in evidence set.
- Fullscreen `.scr` host behavior is not being claimed from BenchLab captures alone.

## Risks And Blockers Relevant To PL14

- Release-cut decisions will need to respect that cross-hardware coverage is still blocked.
- Release messaging must stay within the validated premium subset and must not imply more than `heightfield` for dimensional presentation.
- Transition claims must stay bounded to the implemented supported subset, named compatibility classes, and curated bridge-cycle journeys unless later work gathers more pairwise proof.
- Numeric performance promises remain out of bounds until a later measurement pass exists.
- Multi-monitor claims must stay within the one-session virtual-desktop baseline until later product or host work proves anything broader.
- Environment safety claims must stay within current preview-safe and long-run-stable posture until richer safety modes actually exist.
- Wave 3 expansion must treat the current authored substrate as real but partial rather than assuming a finished authoring suite already exists.

## Grounded Limit Register

| Limit Key | Status | Meaning |
| --- | --- | --- |
| `experimental_pool_bounded` | Partial | `PX30` adds a bounded built-in experimental preset slice, but it is not a broad experimental content catalog and the default pool remains stable-only. |
| `premium_heightfield_only` | Unsupported | `heightfield` is the only implemented dimensional presentation mode. |
| `transition_pair_coverage_bounded` | Partial | Transition proof covers the shipped supported subset, named compatibility classes, and curated bridge-cycle journeys, not every theoretical pair. |
| `benchlab_forcing_bounded` | Partial | BenchLab forcing covers the implemented subset only. |
| `cross_hardware_gap` | Blocked | Proof comes from one capable validation machine rather than a wider hardware sweep. |
| `numeric_perf_sla_gap` | Blocked | PL13 does not claim hard numeric frame-time, memory, or startup SLAs. |
| `multi_monitor_baseline_bounded` | Partial | Current multi-monitor support is the shared virtual-desktop baseline, not independent per-monitor behavior. |
| `environment_safety_bounded` | Partial | Current safety claims are limited to preview-safe, long-run-stable posture, descriptive dark-room content, and truthful clamp or fallback behavior. |
| `authored_registry_partial` | Partial | `PX20` moves sets, journeys, and pack provenance into real files, but the built-in preset and theme descriptor inventory still remains partially code-anchored. |
| `lab_shell_cli_only` | Partial | The first Plasma Lab shell is intentionally CLI-first and support-oriented rather than a live editor. |
| `output_family_subset_bounded` | Partial | `PX30` makes bounded contour and banded output subsets real, but glyph, surface, and broader output breadth remain later work. |
| `treatment_family_subset_bounded` | Partial | `PX30` lands a curated treatment subset with explicit slot and degrade rules, but it does not implement every named treatment family or every theoretical combination. |
| `selection_foundation_bounded` | Partial | Selection now honors authored set weights for bounded fallback behavior, but richer anti-repeat, semantic grouping, and safety-aware logic remain later work. |

## Scope Boundary

These limits are current-repo-grounded facts at the end of `PL13`.
They do not predict or promise what later work will necessarily remove.
