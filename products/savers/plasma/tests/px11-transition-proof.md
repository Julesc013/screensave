# PX11 Transition Proof

## Purpose

This note records the actual evidence and current rerun blocker for the `PX11` transition-coverage tranche.

## Commands And Harness Steps Used

Current repo-available checks used in this checkout:

- `python tools/scripts/check_docs_basics.py`
- `git diff --check`

Transition runtime proof inputs reviewed and extended in-repo:

- `products/savers/plasma/tests/smoke.c`
- `products/savers/plasma/tests/pl13-transition-validation.md`
- `validation/captures/pl13/benchlab-plasma-journey-gl11.txt`

## Supported Direct Morphs Actually Exercised

Checked-in smoke coverage now names and exercises:

- direct theme morph behavior
- direct preset morph behavior
- warm-bridge richer-lane bridge behavior through `warm_bridge_cycle`
- cool-bridge lower-band fallback behavior through `cool_bridge_cycle`
- explicit hard-cut behavior

## Supported Bridge Morphs Actually Exercised

`PX11` adds smoke coverage for:

- richer-lane `warm_bridge_cycle` bridge resolution
- explicit requested-versus-resolved BenchLab reporting for a bridge-morph case

## Fallback And Hard-Cut Cases Actually Exercised

`PX11` adds smoke coverage for:

- lower-band fallback resolution through `cool_bridge_cycle`
- explicit requested-versus-resolved BenchLab reporting for a fallback case
- explicit `hard_cut` policy on an unsupported lower-band preset change

## What Passed, Failed, Or Was Blocked

Passed in this checkout:

- the source-level proof surface was extended
- the validation catalog, known-limit language, and product-local docs were updated consistently
- doc-basics and diff-format checks can be run locally

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no prebuilt Plasma smoke or BenchLab proof binaries were present
- no fresh `validation/captures/px11/` runtime capture could be generated honestly

## Actual Evidence Versus Documentation Only

Actual checked-in evidence:

- prior checked-in `pl13` BenchLab transition capture
- prior checked-in `pl13` transition proof notes
- current source-level smoke assertions that now describe a broader bounded subset

Documentation-only in this checkout:

- any claim that a fresh `PX11` runtime rerun happened locally
- any claim of broader cross-hardware transition coverage

## Current Conclusion

`PX11` leaves a stronger transition proof surface than `PL13`, but it remains bounded and explicitly blocked from claiming fresh rerun evidence in this checkout.
