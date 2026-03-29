# CI Build Notes

CI does not attempt a native ScreenSave build at the current `C03` baseline.

## What CI Checks Now

- repository governance and configuration health
- documentation basics
- build-lane layout and checked-in core/renderer/host/build-file consistency through `tools/scripts/check_build_layout.py`
- current saver/app presence and wiring through the shared platform contracts
- explicit `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` renderer-tier naming across the active build graph
- one true `.scr` output per saver for the current canonical saver set
- BenchLab remaining separate from the distributed saver targets while still exercising the same saver modules directly

## Why CI Does Not Build Yet

The repository now has a real host skeleton, a real shared core runtime, a real baseline GDI renderer, optional GL11 and GL21 capability paths, multiple real saver families, and the first real diagnostics app, but it still lacks a validated native Windows toolchain baseline in CI.
Running a fake native build would still create misleading confidence.

CI should only graduate to real build jobs when later series have enough concrete code for the result to be meaningful.
