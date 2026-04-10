# PX20 Selection Foundation Proof

## Purpose

This note records the actual evidence and current rerun blocker for the bounded selection-foundation work landed by `PX20`.

## Commands And Harness Steps Used

Commands run in this checkout:

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py compare --kind preset-set --left classic_core --right dark_room_classics`

Source-level proof inputs reviewed and extended:

- `products/savers/plasma/src/plasma_selection.c`
- `products/savers/plasma/src/plasma_content.c`
- `products/savers/plasma/tests/smoke.c`

## What Was Exercised

Actual exercised evidence:

- authored preset-set files now carry real non-uniform weights
- compare output shows that those weights differ across sets in meaningful ways

Source-level proof extension:

- `smoke.c` now asserts that weighted fallback on `classic_core` resolves to `plasma_lava`
- `smoke.c` now asserts that excluding `plasma_lava` moves weighted fallback to `quiet_darkroom`

## What Passed, Failed, Or Was Blocked

Passed in this checkout:

- the authored weighting surface is real and validated by the Lab shell
- the weighted fallback logic is checked into the product runtime and smoke surface

Blocked in this checkout:

- no compiler toolchain was available to rerun the new smoke assertions locally
- no persistent anti-repeat runtime exists yet, by design

## Current Conclusion

`PX20` makes weighted authored fallback real, but richer selection intelligence remains later-wave work and the new runtime assertions still need a compiled rerun outside this checkout.
