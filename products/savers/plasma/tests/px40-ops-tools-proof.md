## Purpose

This note records the actual evidence surface for the `PX40` authoring and ops tool tranche beyond the core authored inventory.

## Commands And Harness Steps Actually Used

Commands run in this checkout:

- `python tools/scripts/plasma_lab.py migration-report`
- `python tools/scripts/plasma_lab.py degrade-report --capture validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt`
- `python tools/scripts/plasma_lab.py capture-diff --left validation/captures/pl13/benchlab-plasma-gdi.txt --right validation/captures/pl13/benchlab-plasma-gl11.txt`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`
- `git diff --check`

Proof inputs reviewed and extended in-repo:

- `tools/scripts/plasma_lab.py`
- `validation/captures/pl13/`
- `products/savers/plasma/packs/lava_remix/`

## Validator, Migration, Compare, And Degradation Tools Exercised

The checked-in PX40 ops proof now exercises:

- read-only migration reporting over current authored formats and alias posture
- capture-backed degrade inspection over existing BenchLab text captures
- semantic text-capture comparison over parsed BenchLab report fields
- continued pack-shell SDK validation

## What Passed, Failed, Or Was Blocked

Passed in repo state:

- migration reporting produced a bounded read-only report
- capture-backed degrade inspection produced requested, resolved, degraded, and clamp truth from existing captures
- semantic capture diff produced bounded field-level changes between the GDI and GL11 BenchLab captures

Blocked in this checkout:

- no local compiler toolchain was available to generate fresh compiled captures
- no rendered pixel-diff or image-comparison harness exists

## Actual Evidence Versus Documentation Only

Actual evidence in this checkout:

- working CLI command execution over checked-in authored files and checked-in text captures
- existing BenchLab text captures under `validation/captures/pl13/`

Documentation-only in this checkout:

- any claim of fresh compiled reruns for new captures
- any claim of universal or pixel-perfect deterministic diff across all render paths
