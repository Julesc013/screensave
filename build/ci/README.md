# CI Notes

CI now runs the Proof Kernel v0 compiled Nocturne canary path. It still does
not claim a native Windows release build or compatibility certification.

## What CI Checks Now

- repository governance and configuration health
- documentation basics
- project state, catalog, contracts, product architecture, and VisualIntent
- build-lane layout and checked-in core/renderer/host/build-file consistency through `tools/scripts/check_build_layout.py`
- compiled Nocturne proof-runner build through `tools/scripts/check_compiled_nocturne_runner.py`
- deterministic Proof Kernel v0 render and exact compare through `tools/sslab/sslab.py`
- bounded project-adapter `status`, `capabilities`, `catalog`, `validate`, `render`, and `compare` receipts
- current saver/app presence and wiring through the shared platform contracts
- explicit `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` renderer-tier naming across the active build graph
- one true `.scr` output per saver for the current canonical saver set
- BenchLab remaining separate from the distributed saver targets while still exercising the same saver modules directly

## Why CI Still Does Not Claim A Windows Release Build

The repository now has a real host skeleton, a real shared core runtime, a real baseline GDI renderer, optional GL11 and GL21 capability paths, multiple real saver families, and the first real diagnostics app, but it still lacks a validated native Windows toolchain baseline in CI.
Running a fake native Windows build would still create misleading confidence.

CI should only graduate to release-grade Windows build and PE-audit proof when
the workflow builds the expected `.scr` artifacts and audits a nonempty,
matching artifact manifest. Until then, adapter `proof` remains a local or
future CI command that requires built PE artifacts.
