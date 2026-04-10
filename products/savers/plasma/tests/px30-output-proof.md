# PX30 Output Proof

## Purpose

This note records the actual output-surface evidence for the bounded PX30 contour and banded subset.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_output.c`
- `products/savers/plasma/src/plasma_treatment.c`
- `products/savers/plasma/src/plasma_plan.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/tests/smoke.c`

## Contour Paths Exercised

The checked-in smoke surface now asserts:

- `lava_isolines` resolves to `contour` plus `contour_only`
- `phosphor_topography` resolves to `contour` plus `contour_bands`
- contour plans validate honestly on the bounded supported subset

## Banded Paths Exercised

The checked-in smoke surface now asserts:

- `aurora_bands` resolves to `banded` plus `posterized_bands`
- `stipple_bands` resolves to `banded` plus `posterized_bands`
- `crt_signal_bands` resolves to `banded` plus `posterized_bands`

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- source-level smoke assertions for the new bounded contour and banded subset
- validation-catalog updates for `experimental_pool`, `contour_output_subset`, and `banded_output_subset`
- BenchLab string-surface updates for `posterized_bands`

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh compiled Plasma smoke binary rerun or rendered capture generation was available

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- checked-in runtime implementation
- checked-in smoke assertions
- checked-in validation-catalog updates

Documentation-only in this checkout:

- any claim that a fresh compiled local rerun exercised the updated C smoke binary

## Current Conclusion

`PX30` makes contour and banded real for a bounded experimental subset, but the fresh compiled rerun remains blocked in this checkout.
