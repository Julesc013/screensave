# CI Build Notes

CI does not attempt a native ScreenSave build in Series 10.

## What CI Checks Now

- repository governance and configuration health
- documentation basics
- build-lane layout and checked-in core/renderer/host/build-file consistency through `tools/scripts/check_build_layout.py`
- BenchLab app presence and its wiring to Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, and the shared platform contracts
- GL11 backend presence, conservative WGL setup, and explicit renderer-selection wiring
- Motion Family product presence and narrow multi-product host/build integration
- Framebuffer and Vector Family product presence, narrow shared visual-buffer integration, and continued multi-product host/build integration

## Why CI Does Not Build Yet

The repository now has a real host skeleton, a real shared core runtime, a real baseline GDI renderer, an optional GL11 backend, multiple real saver families, and the first real diagnostics app, but it still lacks a validated native Windows toolchain baseline in CI.
Running a fake native build would still create misleading confidence.

CI should only graduate to real build jobs when later series have enough concrete code for the result to be meaningful.
