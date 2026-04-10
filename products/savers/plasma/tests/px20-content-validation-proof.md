# PX20 Content Validation Proof

## Purpose

This note records the actual validation evidence and current rerun blocker for the `PX20` content and provenance substrate.

## Commands And Harness Steps Used

Commands run in this checkout:

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`

Proof inputs reviewed and extended in-repo:

- `products/savers/plasma/src/plasma_authoring.c`
- `products/savers/plasma/src/plasma_validate.c`
- `products/savers/plasma/packs/lava_remix/pack.ini`
- `products/savers/plasma/packs/lava_remix/pack.provenance.ini`

## What Was Exercised

- authored preset-set headers and member references
- authored theme-set headers and member references
- authored journey headers and set references
- pack provenance header and compatibility fields
- alignment between `pack.provenance.ini` and the shared `lava_remix` `pack.ini`
- shared SDK pack-shell validation over `lava_remix`

## What Passed, Failed, Or Was Blocked

Passed in this checkout:

- `plasma_lab.py validate`
- `check_sdk_surface.py` over the shipped Plasma pack root
- source-level validation catalog updates for the new PX20 substrate entries

Blocked in this checkout:

- no local compiler toolchain was available on `PATH`
- no fresh Plasma smoke binary rerun was available to execute the new C authoring validator path

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- successful Lab validation output
- successful shared SDK pack-shell validation
- checked-in authored files and provenance sidecar

Documentation-only in this checkout:

- any claim that a fresh compiled smoke rerun exercised `plasma_authoring_validate_repo_surface`

## Current Conclusion

`PX20` leaves a real content-validation substrate on disk and in tooling, but the compiled smoke hook remains a rerun blocker in this checkout until a compiler is available.
