# PL13 Known Limits

## Purpose

This document records the current repo-grounded limits, gaps, and caveats for Plasma at the end of `PL13`.
It is factual, current-state documentation, not a wishlist.

## Unsupported Features Or Paths

- `bounded_billboard_volume` remains unsupported for dimensional presentation.
- The repo still does not claim scenic, world-simulated, or free-camera premium behavior.

## Partially Validated Features Or Paths

- Transition proof is bounded to the curated implemented subset, named compatibility classes, and curated bridge-cycle journeys rather than every possible preset or theme pairing.
- The PX30 and PX31 experimental content pool is real but bounded to a small preset slice rather than a broad experimental catalog.
- Contour, banded, and glyph output are now real for bounded subsets rather than taxonomy only, but broader non-raster breadth remains partial.
- Treatment-family support is now real for a curated subset rather than `none` only, but the broader treatment taxonomy remains intentionally partial.
- Field-families I support is now real for bounded chemical, lattice, and caustic studies, but broader field-family-II breadth remains intentionally later work.
- Field-families II support is now real for bounded aurora, substrate, and arc studies, but U05 keeps substrate and arc compatibility-only while the remaining subset stays experimental and intentionally narrow.
- The dimensional subset still extends beyond `heightfield` to bounded `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface`, but U05 keeps only `heightfield` and `ribbon` first-class while broader scenic or billboard-style presentation remains unsupported.
- BenchLab forcing is bounded to the implemented Plasma-specific forcing subset, not every imaginable future knob.
- Performance envelopes are bounded qualitatively rather than with hard numeric SLAs.
- Multi-monitor support is bounded to the shared one-window virtual-desktop baseline rather than independent per-monitor Plasma behavior.
- Environment safety support is bounded to current preview-safe, long-run-stable, descriptive dark-room content, and truthful clamp or fallback behavior.
- The authored substrate is still partial: sets, journeys, and pack provenance are on disk, but built-in preset and theme descriptors are still primarily compiled and legacy-INI anchored.
- The PX40 Plasma Lab surface is now a stronger CLI-first authoring and ops toolchain, but it is still report-first rather than a live editor or suite-grade workstation.
- Deterministic visual proof now combines semantic BenchLab text-capture reports with smoke render signatures and preset-signature audits, but the repo still does not ship a universal screenshot diff framework.
- The PX41 local integration, control, and curation surfaces are now real, but they remain metadata-first and report-first rather than live `suite`, live `anthology`, or remote-automation proof.
- Selection intelligence is still bounded to weighted authored fallback, existing favorites/exclusions, and current set or journey hooks.

## Known Degrade And Fallback Caveats

- Premium presentation requests can degrade honestly back to the compat baseline when the requested path is not available, but PL13 does not claim every theoretical multi-hop forcing combination was exercised live.
- Transition fallback and hard-cut coverage are grounded in the implemented fallback policies, named compatibility classes, and curated bridge-cycle pairs already present in the product, not in speculative future content families.
- Lower-band truth remains the reference floor; richer-lane requests must still be treated as optional uplifts.

## Known Content, Transition, Settings, And BenchLab Limits

- The current repo now ships a bounded experimental preset slice for PX30 and PX31 output, treatment, glyph, and field-family studies, but the default pool remains stable-only and the broader experimental catalog is still absent.
- Settings surfaces are real and validated for the shipped subset, but PL13 does not claim a complete future catalog of every eventual product control.
- Settings influence is now backed by deterministic smoke render-signature checks for the shipped subset, but the thresholds remain a bounded dead-setting detector rather than a perceptual gold-standard lab.
- BenchLab reporting and forcing are real for the implemented subset, but PL13 does not claim a full product debugger or unrestricted forcing console.
- PX40 capture-backed degrade inspection and capture diff are real for the current BenchLab text surface, but PL13 does not claim universal deterministic replay or rendered capture parity across every lane.
- PX41 local projection, control, and curation reports are real for the implemented subset, but PL13 does not claim live consumer ingestion, cross-product favorites behavior, remote automation, or a community platform.

## Test-Environment Gaps

- The current capture set comes from one capable validation machine.
- PL13 does not include a wider cross-driver, cross-GPU, or low-capability denial sweep.
- Mixed-topology and Plasma-specific preview-host reruns are not part of the current checked-in evidence set.
- Fullscreen `.scr` host behavior is not being claimed from BenchLab captures alone.

## Risks And Blockers Relevant To PL14

- Release-cut decisions will need to respect that cross-hardware coverage is still blocked.
- Release messaging must stay within the validated bounded dimensional subset and must not imply billboard-style or scenic presentation.
- Transition claims must stay bounded to the implemented supported subset, named compatibility classes, and curated bridge-cycle journeys unless later work gathers more pairwise proof.
- Numeric performance promises remain out of bounds until a later measurement pass exists.
- Multi-monitor claims must stay within the one-session virtual-desktop baseline until later product or host work proves anything broader.
- Environment safety claims must stay within current preview-safe and long-run-stable posture until richer safety modes actually exist.
- Wave 3 expansion must treat the current authored substrate as real but partial rather than assuming a finished authoring suite already exists.
- Later-wave glyph breadth, field-families II, and broader dimensional work must build on the bounded PX31 subset rather than overclaiming a finished non-raster or field-language stack.
- Wave 4 tooling work must keep capture comparison semantic and text-based unless a later proof pass can honestly widen that surface.
- Later proof work must treat the new render-signature harness as bounded support evidence rather than as a substitute for broader cross-hardware screenshot capture.
- Wave 4 local ecosystem work must keep integration, control, and curation claims metadata-first and report-first unless a later proof pass can honestly widen them.

## Grounded Limit Register

| Limit Key | Status | Meaning |
| --- | --- | --- |
| `experimental_pool_bounded` | Partial | `PX30` and `PX31` add a bounded built-in experimental preset slice, but it is not a broad experimental content catalog and the default pool remains stable-only. |
| `dimensional_presentation_subset_bounded` | Partial | The admitted dimensional subset still covers `heightfield`, `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface`, but U05 keeps only `heightfield` and `ribbon` first-class while broader scenic or billboard-style presentation remains unsupported. |
| `transition_pair_coverage_bounded` | Partial | Transition proof covers the shipped supported subset, named compatibility classes, and curated bridge-cycle journeys, not every theoretical pair. |
| `benchlab_forcing_bounded` | Partial | BenchLab forcing covers the implemented subset only. |
| `cross_hardware_gap` | Blocked | Proof comes from one capable validation machine rather than a wider hardware sweep. |
| `numeric_perf_sla_gap` | Blocked | PL13 does not claim hard numeric frame-time, memory, or startup SLAs. |
| `multi_monitor_baseline_bounded` | Partial | Current multi-monitor support is the shared virtual-desktop baseline, not independent per-monitor behavior. |
| `environment_safety_bounded` | Partial | Current safety claims are limited to preview-safe, long-run-stable posture, descriptive dark-room content, and truthful clamp or fallback behavior. |
| `authored_registry_partial` | Partial | `PX20` moves sets, journeys, and pack provenance into real files, but the built-in preset and theme descriptor inventory still remains partially code-anchored. |
| `lab_shell_cli_only` | Partial | `PX40` broadens the Lab shell into a stronger CLI-first authoring and ops surface, but it is still report-first rather than a live editor, gallery, or suite-grade workstation. |
| `capture_diff_bounded` | Partial | `PX40` adds semantic BenchLab text-capture comparison and capture-backed degrade inspection, but it does not claim raw-text identity or pixel-perfect determinism across every lane. |
| `render_signature_harness_bounded` | Partial | U07 adds deterministic smoke render-signature and pixel-difference checks for the shipped settings subset, but it does not claim a universal screenshot diff framework or perceptual gold-standard QA across every lane. |
| `integration_metadata_first` | Partial | `PX41` adds local projection, control, and curation metadata plus report surfaces, but it does not claim live `suite` or `anthology` ingestion or remote automation behavior. |
| `curation_surface_local_bounded` | Partial | `PX41` adds local curated collections and a one-pack provenance index, but it does not claim ratings, dislikes, broad provenance discovery, or a community platform. |
| `output_family_subset_bounded` | Partial | `PX30` and `PX31` make bounded contour, banded, and glyph output subsets real, but surface output and broader output breadth remain later work. |
| `treatment_family_subset_bounded` | Partial | `PX30` lands a curated treatment subset with explicit slot and degrade rules, but it does not implement every named treatment family or every theoretical combination. |
| `glyph_output_subset_bounded` | Partial | `PX31` lands bounded `ascii_glyph` and `matrix_glyph` modes, but it does not implement a broad text renderer, larger glyph alphabets, or every treatment and transition combination. |
| `field_family_i_subset_bounded` | Partial | `PX31` lands bounded `chemical_cellular_growth`, `lattice_quasi_crystal`, and `caustic_marbling` studies, but it does not implement field-families II, broader dimensional work, or exhaustive lane proof for those families. |
| `field_family_ii_subset_bounded` | Partial | `PX32` lands bounded `aurora_curtain_ribbon`, `substrate_vein_coral`, and `arc_discharge` studies, but U05 keeps substrate and arc compatibility-only and does not implement scenic breadth, broader ecosystem work, or exhaustive lane proof for those families. |
| `selection_foundation_bounded` | Partial | Selection now honors authored set weights for bounded fallback behavior, but richer anti-repeat, semantic grouping, and safety-aware logic remain later work. |

## Scope Boundary

These limits are current-repo-grounded facts at the end of `PL13`.
They do not predict or promise what later work will necessarily remove.
