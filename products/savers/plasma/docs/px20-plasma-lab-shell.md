# PX20 Plasma Lab Shell

## Purpose

This note records what the first real `Plasma Lab` shell means in the repo after `PX20`.

## What “Minimal Plasma Lab Shell” Means Now

The current Lab shell is intentionally file-first and CLI-oriented.
It is implemented as:

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py compare ...`
- `python tools/scripts/plasma_lab.py degrade-report ...`

## What It Can Do Now

- validate authored preset sets, theme sets, journeys, and pack provenance
- cross-check `lava_remix` against the shared SDK pack-shell validator
- compare authored preset sets, theme sets, and journeys textually
- print a bounded degrade report for the current pack shell

## What It Explicitly Cannot Do Yet

- it cannot live-edit files
- it cannot render previews or perform image-based diffs
- it cannot act as a full GUI editor
- it cannot discover broad portable or user authoring roots
- it cannot replace BenchLab or the runtime validation surface

## Relationship To Later Authoring Work

The current Lab shell is a foothold, not a finished authoring suite.
Later work may broaden it, but `PX20` only claims the bounded textual shell above.
