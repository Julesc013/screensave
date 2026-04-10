# PX41 Integration Proof

## Purpose

This note records the actual commands used to exercise the bounded `PX41` local integration, SDK/reference, and control surfaces.

## Commands Used

```text
python -m py_compile tools/scripts/plasma_lab.py
python tools/scripts/check_docs_basics.py
python tools/scripts/plasma_lab.py validate
python tools/scripts/plasma_lab.py integration-report
python tools/scripts/plasma_lab.py control-report --profile deterministic_classic
python tools/scripts/plasma_lab.py control-report --profile experimental_matrix
python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix
git diff --check
```

## Anthology, Suite, SDK, And Automation Surfaces Exercised

- `projection_surface.ini`
- `sdk_reference.ini`
- `control_profiles.ini`
- `integration-report`
- `control-report`
- `lava_remix` SDK pack-shell compatibility

## What Passed, Failed, Or Was Blocked

- Passed: repo-side validation of the new metadata surfaces and their references against the current compiled and authored Plasma catalog
- Passed: stable-only and experimental-only control-profile posture checks
- Passed: SDK shell validation for the existing built-in pack
- Blocked: no fresh compiled `suite`, `anthology`, or saver runtime rerun was available in this checkout for live consumer integration

## Evidence Versus Documentation Only

Actual evidence in this tranche is:

- the checked-in metadata files
- the expanded Plasma Lab validation and report commands
- the command outputs from the commands above

Documentation-only claims avoided:

- live `suite` consumption
- live `anthology` consumption
- remote automation behavior
