# CI Build Notes

CI does not attempt a native ScreenSave build in Series 02.

## What CI Checks Now

- repository governance and configuration health
- documentation basics
- build-lane layout and checked-in build-file consistency through `tools/scripts/check_build_layout.py`

## Why CI Does Not Build Yet

The repository still lacks real host, renderer, and saver implementation.
Running a fake native build would create misleading confidence.

CI should only graduate to real build jobs when later series have enough concrete code for the result to be meaningful.
