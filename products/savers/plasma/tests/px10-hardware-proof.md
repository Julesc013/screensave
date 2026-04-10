# PX10 Hardware Proof

## Purpose

This note records the actual evidence sources and actual rerun blockers used for the `PX10` hardware-proof pass.

## Evidence Sources Actually Used

Actual checked-in evidence reviewed for this tranche:

- [`validation/captures/sx09/benchlab-plasma-auto.txt`](../../../../validation/captures/sx09/benchlab-plasma-auto.txt)
- [`validation/captures/sx09/benchlab-plasma-gl46.txt`](../../../../validation/captures/sx09/benchlab-plasma-gl46.txt)
- [`validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt)
- [`validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt)
- [`validation/captures/pl13/benchlab-plasma-gl21.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl21.txt)
- [`validation/captures/pl13/benchlab-plasma-gl33.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl33.txt)
- [`validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt)
- [`validation/notes/c05-windows-integration-matrix.md`](../../../../validation/notes/c05-windows-integration-matrix.md)
- [`validation/notes/ss02-support-matrix.md`](../../../../validation/notes/ss02-support-matrix.md)

## Product And Harness Surfaces Reviewed

Actual product-local surfaces reviewed for the PX10 matrix:

- [`../src/plasma_validate.c`](../src/plasma_validate.c)
- [`../src/plasma_sim.c`](../src/plasma_sim.c)
- [`./smoke.c`](./smoke.c)

## Environments Actually Exercised In Checked-In Evidence

- Windows 10 Enterprise `2009` x64 release-validation environment for the shared host path
- one capable validation machine whose OpenGL captures report:
  - vendor `NVIDIA Corporation`
  - renderer `NVIDIA GeForce RTX 2070/PCIe/SSE2`
  - driver string `gl=4.6.0 NVIDIA 591.86 ...`

## What Passed

- forced `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` report-mode runs on the known validation machine
- `auto` and forced `gl46` policy-cap truth on the known validation machine
- bounded fullscreen host-path evidence on Windows 10 x64

## What Remains Partial, Blocked, Or Not Tested

- Plasma-specific preview-host rerun remains partial because the current checked-in host audit is shared-host evidence rather than a fresh PX10 rerun
- mixed physical-monitor topology remains not tested
- lower-capability denial proof across another driver or GPU remains blocked

## What Was Observed Versus Only Documented

Observed in checked-in evidence:

- lane selection and policy-cap truth
- backend vendor and driver strings on the known capable machine
- real host-path startup and close behavior on Windows 10 x64

Only documented or source-audited in this tranche:

- the new explicit product-local matrix shape
- the explicit split between partial, blocked, and not-tested breadth

## Local Rerun Blockers In This Checkout

- No local compiler toolchain was available on `PATH`.
- The toolchain-pinned WinLibs path recorded in `PL13` was not present in this checkout.
- No prebuilt `plasma_smoke.exe` or `benchlab.exe` was available under `out/`.

Because of that, PX10 did not fabricate fresh hardware captures.
It tightened the product-local hardware matrix around the real evidence already in the repo.
