# SX09 Substrate Validation Summary

Date: 2026-04-08
Series: SX09
Scope: finished SX substrate ladder validation, BenchLab routing observability, deterministic report capture, and PL handoff readiness

## Evidence Anchor

- Proof matrix: [sx09-tier-proof-matrix.md](./sx09-tier-proof-matrix.md)
- Capture index: [../captures/sx09/README.md](../captures/sx09/README.md)
- PL handoff: [../../docs/roadmap/pl00-substrate-handoff.md](../../docs/roadmap/pl00-substrate-handoff.md)
- Routing doctrine: [../../specs/routing_policy.md](../../specs/routing_policy.md)
- Backend and seam doctrine: [../../specs/backend_policy.md](../../specs/backend_policy.md)

## Environment

| Field | Value |
| --- | --- |
| OS | `Microsoft Windows 10 Enterprise` |
| Windows version | `2009` |
| Build | `10.0.19041.6456` |
| Architecture | `64-bit` on `x64-based PC` |
| PowerShell | `5.1.19041.7058` |
| GPU adapter | `NVIDIA GeForce RTX 2070/PCIe/SSE2` |
| OpenGL driver seen in modern and premium proof | `4.6.0 NVIDIA 591.86` |

## What SX09 Validated

- The MinGW i686 debug lane still builds the shared platform library, the checked-in saver line, BenchLab, and Suite after the SX ladder, seam, and routing work.
- BenchLab now emits bounded deterministic text reports that capture requested renderer, policy target, selected renderer and band, degraded path, fallback cause, backend detail, overlay snapshot, and recent diagnostics.
- `gallery` now has real bounded proof reports for `gdi`, `gl11`, `gl21`, `gl33`, and `gl46`.
- `gallery` auto-routing now proves that saver policy can prefer `gl33` on a machine that also supports `gl46`; the substrate does not silently jump to the highest available tier when saver policy says otherwise.
- `nocturne` and `plasma` auto-routing now prove that ordinary non-showcase savers still route honestly through the compat lane without learning backend-private trivia.
- Forced `gl46` requests on `nocturne` and `plasma` now prove that saver policy clamp behavior is observable: requested, policy target, and active renderer are all visible in the resulting reports.
- The MinGW build lane now records header dependencies for C objects, closing the stale-object risk that SX09 uncovered while validating the larger private renderer struct.

## Deterministic Capture Support

- BenchLab report mode now supports `/report` and `/frames:<n>` with deterministic runs driven by `/deterministic /seed:<n>`.
- The SX09 capture set was generated with `seed=743` and `frames=4` to keep reports short, repeatable, and comparable.
- Each report records:
  - requested renderer
  - policy target
  - active renderer
  - selected band
  - degraded path
  - fallback cause
  - backend detail string
  - overlay snapshot
  - recent diagnostics
- GDI report mode now presents into an offscreen compatible memory DC during hidden BenchLab runs so universal-lane proof does not fail on a hidden child-window `BitBlt`.

## Validation Weakness Found And Resolved

- SX09 uncovered a real build-truth issue in the MinGW lane: private-header growth could leave older backend objects in place because the checked-in makefile did not track header dependencies.
- That stale-object state manifested as heap corruption during `screensave_renderer_init_dispatch` when newer renderer-private fields were written into renderer blocks allocated by older backend objects.
- SX09 resolved that by adding automatic `.d` dependency generation and inclusion to `build/mingw/i686/Makefile`, then confirming that a touched private renderer header now triggers the expected dependent rebuilds.

## Honest Limits

- The bounded proof set is real, but it is still BenchLab windowed-harness evidence rather than exhaustive fullscreen `.scr` host proof for every saver and every tier.
- The current machine supports the full GL ladder exercised here, so SX09 did not obtain a live “premium unavailable” or “modern unavailable” capability-fallback capture; those downgrade paths remain implemented and observable, but were not freshly triggered in this environment.
- The reports demonstrate deterministic control and repeatable harness capture, not cross-driver pixel identity for every GL implementation.
- `gallery` remains the renderer-showcase proof surface. `plasma` remains the first `PL` flagship product and was not turned into the main SX proof surface.

## Outcome

SX09 closes the SX series with real system-level evidence rather than code-only confidence.
The repo now contains:

- a bounded proof set for the full public ladder
- clearer requested-versus-policy-target-versus-selected routing visibility
- deterministic BenchLab report capture
- a written PL handoff that distinguishes guaranteed substrate behavior from bounded or still-open areas

That is enough to start `PL00` from a documented substrate contract instead of re-deriving renderer architecture during Plasma work.
