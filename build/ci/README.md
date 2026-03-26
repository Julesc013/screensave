# CI Build Notes

CI does not attempt a native ScreenSave build in Series 04.

## What CI Checks Now

- repository governance and configuration health
- documentation basics
- build-lane layout and checked-in core/host/build-file consistency through `tools/scripts/check_build_layout.py`

## Why CI Does Not Build Yet

The repository now has a real host skeleton and a real shared core runtime, but it still lacks a validated toolchain baseline in CI, reusable renderer backends, and real saver-product behavior.
Running a fake native build would still create misleading confidence.

CI should only graduate to real build jobs when later series have enough concrete code for the result to be meaningful.
