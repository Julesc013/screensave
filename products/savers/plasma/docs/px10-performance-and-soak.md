# PX10 Performance And Soak

## Purpose

This document tightens Plasma's current performance-envelope posture without inventing hard numeric promises.
It separates what the repo has actually measured or counted from what is only qualitatively bounded and from what remains unknown.

## Terminology Used

`PX10` uses the `SY10` performance and soak vocabulary:

- `startup`: session creation and compile path reaching a runnable state
- `session_compile`: the product-local plan and session build path
- `runtime_envelope`: the currently supportable runtime-behavior description
- `restart_soak`: repeated create, step, render, and destroy coverage
- `transition_soak`: repeated bounded transition coverage
- `long_run_soak`: unattended longer-duration posture

Evidence classes in this document are:

- `measured`: exact counts, deterministic run lengths, or direct pass/fail outcomes exist in the repo
- `bounded_qualitative`: the repo proves a bounded behavior class, but not hard numeric timing or memory limits
- `unknown`: the repo does not currently have honest measured evidence for the claim

## Measured Directly

The repo currently measures or counts these facts directly:

| Surface | Evidence Class | Evidence | Meaning |
| --- | --- | --- | --- |
| Forced lane report runs | `measured` | [`../../../../validation/captures/pl13/README.md`](../../../../validation/captures/pl13/README.md) | The repo contains deterministic four-frame BenchLab reports for forced `gdi`, `gl11`, `gl21`, `gl33`, and `gl46 heightfield` runs. |
| Policy-cap degrade run | `measured` | [`../../../../validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`](../../../../validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt) | The repo contains a deterministic report proving requested premium presentation degraded honestly back to the lower-band path. |
| Lower-band restart soak loop length | `measured` | [`../tests/pl13-soak-and-proof.md`](../tests/pl13-soak-and-proof.md), [`../tests/smoke.c`](../tests/smoke.c) | The checked-in smoke harness uses twelve repeated lower-band create, step, render, and destroy iterations. |
| Journey-transition soak loop length | `measured` | [`../tests/pl13-transition-validation.md`](../tests/pl13-transition-validation.md), [`../tests/smoke.c`](../tests/smoke.c) | The checked-in smoke harness uses eight repeated journey-driven transition iterations on `gl21`. |
| Longer GL11 journey capture | `measured` | [`../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt) | The repo contains a deterministic 320-frame journey capture on the preferred lower-band lane. |

## Qualitatively Bounded

These claims remain honest and supportable, but still qualitative:

| Envelope | Evidence Class | Evidence | Current Meaning |
| --- | --- | --- | --- |
| `default_path_gdi` | `bounded_qualitative` | [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt) | The preserved default path compiles, starts, and runs on the mandatory floor without richer-lane dependence. |
| `default_path_gl11` | `bounded_qualitative` | [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | The preferred stable accelerated default lane remains honest and bounded. |
| `advanced_gl21` | `bounded_qualitative` | [`../../../../validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt) | The bounded advanced component subset is real, but PX10 does not convert that into a broader cost promise. |
| `modern_gl33` | `bounded_qualitative` | [`../../../../validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt) | The bounded modern subset is real, but not numerically characterized. |
| `premium_gl46_heightfield` | `bounded_qualitative` | [`../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt) | The current premium subset plus `heightfield` is real, but still bounded and not given hard cost limits. |
| `transition_subset` | `bounded_qualitative` | [`../tests/pl13-transition-validation.md`](../tests/pl13-transition-validation.md) | Theme morph, preset morph, bridge, fallback, and journey behavior are supportable only for the implemented curated subset. |

Legacy proof callers may still resolve `classic_gdi` and `classic_gl11`, but
the canonical envelope names now describe the preserved default path directly.

## Startup And Session-Compile Notes

- The strongest actual startup evidence remains successful deterministic report-mode startup on the known validation machine.
- The strongest actual session-compile evidence remains the checked-in smoke harness and the deterministic BenchLab reports from `PL13`.
- `PX10` does not add hard startup timing numbers because the repo still does not contain an honest numeric timing harness.

## Soak Coverage

### Restart Soak

- Lower-band restart soak is grounded in the repeated `gdi` and `gl11` create, step, render, and destroy loop in the smoke harness.
- This is strongest on the stable default Classic path.
- It proves repeatability and bounded stability, not a frame-time SLA.

### Transition Soak

- Transition soak is grounded in repeated journey-driven stepping on `gl21` plus the longer `gl11` journey capture.
- It proves the implemented transition subset is real and can be exercised repeatedly.
- It does not prove exhaustive pairwise cost or every theoretical transition path.

### Long-Run Posture

- The saver-level `long_run_stable=1` claim remains real and should be read together with the bounded soak evidence above.
- `PX10` strengthens the wording around long-run posture, but the repo still does not contain a dedicated long-duration multi-hour harness.

## Transition-Cost Notes

- Current transition-cost evidence is qualitative only.
- The repo proves that meaningful transition behavior exists on lower and richer lanes for the curated subset.
- The repo does not prove hard transition timing targets or memory ceilings.

## Remaining Envelope Caveats

- No hard numeric frame-time SLA exists.
- No hard memory ceiling exists.
- No per-driver numeric variance study exists.
- No separate long-duration advanced, modern, or premium soak matrix exists.
- BenchLab captures remain stronger for requested-versus-resolved truth than for final fullscreen host behavior.

## Uncertainty Sources

- one capable validation machine dominates the current evidence
- no dedicated numeric timing harness is checked in
- no dedicated long-run soak harness beyond smoke and deterministic report-mode evidence is checked in
- richer-lane support remains intentionally broader in implementation than in current support proof

## Scope Boundary

`PX10` makes the current envelope posture more explicit.
It does not create hard performance SLAs or a new benchmarking architecture.
