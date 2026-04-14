# PL13 Performance Envelopes

## Purpose

This document records Plasma's current performance and stability envelopes for the implemented subset.
The emphasis is on bounded, evidence-backed operating notes rather than invented hard numbers.

## What "Performance Envelope" Means Here

For Plasma in `PL13`, a performance envelope is the currently honest combination of:

- supported lane or backend path
- bounded startup and session-compilation behavior
- bounded runtime-responsiveness expectations
- bounded transition-cost expectations
- bounded soak or repeatability expectations
- explicit uncertainty notes

## Scope Of Measured Or Bounded Areas

PL13 covers:

- forced `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` BenchLab report-mode runs
- repeated session creation and destruction on the lower bands in smoke
- repeated bounded journey-transition stepping on `gl21` in smoke
- degrade proof from requested premium presentation back to the compat baseline

PL13 does not claim:

- cross-machine numeric performance baselines
- hard frame-time SLAs
- hard memory ceilings
- exhaustive long-duration richer-lane soak proof

## Lane Envelopes

| Envelope | Status | Evidence | Startup / Compile Notes | Runtime / Transition Notes | Soak Notes |
| --- | --- | --- | --- | --- | --- |
| `default_path_gdi` | Validated | [`validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt) | Preserved default-path session creation and report-mode startup succeeded on forced `gdi`. | Runtime surface remains the preserved default raster-plus-flat path with no richer-lane uplift. | Repeated lower-band create/destroy plus stepping is covered in smoke. |
| `default_path_gl11` | Validated | [`validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | Preserved default-path session creation and report-mode startup succeeded on forced `gl11`. | Compat lane remains the stable accelerated default baseline; shared bounded transition subset is also routable here. | Repeated lower-band create/destroy plus stepping is covered in smoke. |
| `advanced_gl21` | Validated | [`validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt) | Advanced lane compiles and selects successfully in BenchLab report mode. | Envelope is limited to the bounded PL07 component subset and shared transition engine. | No separate long-duration advanced-only soak is claimed. |
| `modern_gl33` | Validated | [`validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt) | Modern lane compiles and selects successfully in BenchLab report mode. | Envelope is limited to the bounded PL08 refined subset and flat presentation continuity. | No separate long-duration modern-only soak is claimed. |
| `premium_gl46_heightfield` | Validated | [`validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt) | Premium lane compiles and selects successfully in BenchLab report mode. | Envelope is limited to the bounded premium subset plus `heightfield`; no broader dimensional family is claimed. | No unrestricted heavy-feedback or scenic premium soak claim is made. |
| `transition_subset` | Partial | [`../tests/pl13-transition-validation.md`](../tests/pl13-transition-validation.md) | Transition-capable sessions compile and start for the implemented subset. | Theme morph, preset morph, bridge morph, fallback, and journey behavior are covered only for curated supported content. | Repeated bounded stepping is covered, but exhaustive pairwise soak is not claimed. |

## Startup And Session Compilation Notes

- All forced-lane BenchLab captures completed successfully on the current validation machine.
- Smoke continues to prove default session creation, runtime-plan validation, and lower-band repeated create/destroy behavior.
- Requested premium presentation on auto policy is clamped and reported rather than leaking an impossible path into runtime.
- Legacy proof callers may still resolve `classic_gdi` and `classic_gl11`, but
  `U01` freezes `default_path_gdi` and `default_path_gl11` as the canonical
  envelope keys.

## Runtime Stability And Responsiveness Notes

- The strongest runtime claims remain qualitative and bounded.
- The preserved default path on `gdi` and `gl11` remains the most strongly exercised stability surface.
- Richer lanes are validated as real selectable paths, but not yet given independent numeric responsiveness targets.

## Transition Cost Notes

- Transition cost is validated only for the implemented subset.
- The GL11 journey capture proves that meaningful transition behavior does not require premium hardware.
- The smoke soak uses real journey dwell values and bounded follow-up ticks so bridge transitions and direct morphs are both exercised honestly.

## Soak Expectations For The Validated Subset

- Lower-band stability expectation: repeated create/destroy and repeated stepping succeed without invalid plan drift.
- Transition stability expectation: bounded repeated journey-driven transitions succeed for the curated supported subset.
- Richer-lane expectation: report-mode startup and bounded proof runs succeed; no broader long-duration soak promise is made yet.

## Uncertainty Sources And Gaps

- Captures were recorded on one capable validation machine.
- PL13 does not provide a multi-driver or low-capability denial matrix.
- PL13 does not establish numeric frame-time, memory, or startup SLAs.
- Premium-dimensional coverage is intentionally narrow because only `heightfield` is implemented.

## Measurement Status

- Measured directly: successful deterministic BenchLab report-mode runs and smoke pass/fail outcomes.
- Estimated qualitatively: lane-specific responsiveness and cost envelopes.
- Not yet known: hard numeric SLAs, wider hardware variance, and exhaustive long-run richer-lane behavior.
