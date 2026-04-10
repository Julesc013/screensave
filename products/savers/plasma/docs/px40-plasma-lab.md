# PX40 Plasma Lab

## Purpose

This note records what `Plasma Lab` means in the repo after `PX40`.

## Why PX40 Exists Now

`PX20` landed the first authored substrate and a minimal Lab shell.
`PX30` through `PX32` widened Plasma's visual language, which made the product harder to validate, compare, and support with the earlier three-command shell alone.
`PX40` completes the first serious authoring-facing and ops-facing Lab surface without turning Plasma into a suite or platform project.

## Relationship To PX20 And Later Wave 4 Work

`PX20` created the first bounded foothold:

- authored preset sets
- authored theme sets
- authored journeys
- `lava_remix` provenance
- `validate`, `compare`, and pack `degrade-report`

`PX40` keeps that file-first and CLI-first posture, but makes it materially more useful for day-to-day authoring and support work.
Later Wave 4 ecosystem work may consume these local reports, but `PX40` does not define shared suite law and does not build `SY40` or `PX41`.

## What “Plasma Lab” Means After PX40

After `PX40`, `Plasma Lab` is the bounded product-local CLI at `python tools/scripts/plasma_lab.py`.

It now exposes these real entry points:

- `validate`
- `authoring-report`
- `compare`
- `compat-report`
- `migration-report`
- `degrade-report`
- `capture-diff`

## Actual Authoring Surfaces Landed

The current Lab surface can now:

- validate authored preset sets, theme sets, journeys, and pack provenance against the current compiled catalog
- report the current authored-versus-compiled boundary explicitly
- compare authored preset sets, theme sets, and journeys textually
- report compatibility for authored sets, journeys, and the shipped pack shell
- report alias and migration posture without rewriting files
- inspect degrade truth from the shipped pack shell or from existing BenchLab text captures
- compare deterministic BenchLab text captures semantically rather than pretending to do rendered image diffs

## Actual Workflows Supported Now

The truthful current workflows are:

- authoring audit: `validate` plus `authoring-report`
- authored-object comparison: `compare`
- compatibility inspection: `compat-report`
- key and migration inspection: `migration-report`
- degrade inspection: `degrade-report`
- deterministic text-capture comparison: `capture-diff`

## What Plasma Lab Explicitly Cannot Do Yet

It still cannot:

- live-edit presets, themes, sets, journeys, or packs
- act as a GUI editor or gallery browser
- render previews or promise pixel-perfect output diffs
- rewrite older authored files automatically
- discover a broad user-root or portable authoring ecosystem
- replace BenchLab or the runtime validation surface

## Settings And BenchLab Truth Where Relevant

`PX40` does not redesign settings or BenchLab.
The Lab only consumes existing truth:

- authored profile scopes and content channels from the current product-local content layer
- pack routing and migration policy from `pack.ini` plus `pack.provenance.ini`
- BenchLab requested, resolved, degraded, clamp, and forcing fields from existing text captures

## Scope Boundary

`PX40` completes a bounded CLI-first authoring and ops surface.
It is not a giant editor, not a suite/platform contract, and not Wave 4 ecosystem integration work.
