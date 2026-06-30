# Plasma v2 Publication Hold

Status: Plasma v2 publication is blocked by real-display visual rejection.

Date recorded: 2026-07-01.

## Decision

Do not publish Plasma v2. Preserve ScreenSave. Restart Plasma's visual core.

The earlier publication-prep packet remains useful historical evidence that the
local release process ran and stayed non-public. It is not product acceptance.
A screensaver's primary acceptance test is visual, and the current tested
Plasma artifact was rejected as low-resolution, blocky, stuttery, and not
visually acceptable on real displays.

## Current State

```toml
[plasma_v2]
status = "publication-hold"
stable = false
stable_promotion_historical = "accepted"
current_product_verdict = "visual-rejected"
release_promotion = "withdrawn-for-visual-quality"
publication_prep = "superseded"
publication = "not-published"
opened_next = "plasma-v3-visual-core-spike"
visual_blocker = "current artifact fails real-display visual acceptance"
```

The exact rejection verdict is recorded at
`validation/captures/plasma-v2/visual-rejection/verdict.toml`.

## Preserve

Keep ScreenSave's surrounding product system:

- native Win32 `.scr` product model;
- ANSI Win32 host path;
- mandatory GDI floor;
- optional capability-gated GL ladder;
- evidence-classed compatibility language;
- catalog, manifest, artifact, and proof surfaces;
- AIDE as bounded development and evidence infrastructure only.

## Restart

Restart the Plasma visual engine rather than polishing the v2 themes or adding
more preset conditionals. The V3 spike should build one beautiful deterministic
field first, with direct controls, a C89-compatible software/reference path,
smooth presentation, contact-sheet proof, frame-time evidence, and an explicit
human visual verdict before any publication reconsideration.

## Gate Rule

Publication gates must hold while
`validation/captures/plasma-v2/visual-rejection/verdict.toml` records
`decision = "reject-publication"`.

Green package, checksum, provenance, or publication-prep evidence is not enough
to ship Plasma when the product verdict is visually rejected. PAW-K remains
blocked until a replacement path receives an accepted real-display visual
verdict.
