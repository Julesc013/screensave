# PX10 Soak Proof

## Purpose

This note records the actual soak and envelope evidence used for `PX10`, plus the local rerun blockers for this checkout.

## Evidence Sources Actually Used

Actual checked-in evidence reviewed for this tranche:

- [`../tests/pl13-soak-and-proof.md`](./pl13-soak-and-proof.md)
- [`../tests/pl13-transition-validation.md`](./pl13-transition-validation.md)
- [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt)
- [`../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt)

## Product And Harness Surfaces Reviewed

- [`../src/plasma_validate.c`](../src/plasma_validate.c)
- [`../src/plasma_sim.c`](../src/plasma_sim.c)
- [`./smoke.c`](./smoke.c)

## Soak Scenarios Actually Represented In Repo Evidence

- lower-band repeated create, step, render, and destroy coverage on `gdi` and `gl11`
- deterministic forced-lane report-mode runs on `gdi`, `gl11`, `gl21`, `gl33`, and `gl46`
- deterministic degrade proof from premium-only presentation request back to the compat baseline
- repeated journey-driven transition stepping on `gl21`
- a longer deterministic `gl11` journey capture for the bounded lower-band transition subset

## Non-Fatal Observations

- The strongest current evidence is still deterministic report-mode truth plus smoke pass and fail coverage.
- Lower-band and transition soak coverage are real, but still bounded rather than exhaustive.
- The repo still distinguishes requested, resolved, and degraded runtime truth more strongly than it distinguishes numeric performance cost.

## What Is Still Unknown

- hard numeric startup time
- hard numeric frame-time
- hard memory ceilings
- broader long-duration advanced, modern, or premium soak behavior
- broader cross-hardware variance

## Local Rerun Blockers In This Checkout

- No compiler toolchain was available on `PATH`.
- The exact WinLibs toolchain path recorded in `PL13` was not present locally.
- No prebuilt smoke or BenchLab binaries were present under `out/`.

Because of that, PX10 did not add fabricated numeric or soak captures.
It tightened the repo-grounded split between measured counts, bounded qualitative evidence, and unknowns.
