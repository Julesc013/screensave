# U03 Settings Surfaces

## Purpose

This note records the `U03` settings-surface redesign.

It freezes the exact Basic, Advanced, and Author/Lab split that now ships in
the Plasma dialog after `U01` unified the runtime and `U02` made settings the
real grammar authority.

## Why U03 Exists Now

Before `U03`, the repo had the right settings catalog and a partially truthful
dialog, but the shipped controller still behaved like a flat mixed-scope
surface:

- higher surfaces behaved cumulatively instead of exactly
- routine refreshes reread hidden controls back into the working config
- the dialog exposed deeper state poorly or not at all
- the layout still looked like one long historical sheet instead of three
  bounded surfaces

`U03` fixes the control surface.
It does not claim deeper output, treatment, or preset retuning work.

## Final Split Implemented

### Basic

Basic is now the small stable first-pass surface.

It owns:

- preset
- theme
- speed
- visual intensity
- content pool
- allow transitions
- safe defaults

Basic no longer leaks generator, output, treatment, or authoring controls into
the default user path.

### Advanced

Advanced is now the grammar surface for users who want direct control over the
current runtime plan.

It owns:

- generator family
- output family
- output mode
- resolution
- smoothing
- filter treatment
- emulation treatment
- accent treatment
- presentation mode
- deterministic mode

Advanced is exact.
It does not include the Basic controls as a second copy.

### Author/Lab

Author/Lab is now the bounded curation and diagnostics surface.

It owns:

- preset set
- theme set
- favorites only
- transition policy
- journey
- fallback policy
- seed continuity
- interval
- duration
- fixed seed value
- diagnostics toggle

Author/Lab is also exact.
It does not act like a superset of Basic and Advanced.

## What Each Surface Is For

- `Basic` keeps the stable default path supportable and legible.
- `Advanced` exposes the real visual grammar without pretending it is still
  preset-owned.
- `Author/Lab` keeps curation, journey, transition, and diagnostics controls
  out of the normal user path while still preserving them as real product
  surfaces.

## What Remains Hidden Or Deferred

The following surfaces remain intentionally hidden or deferred after `U03`:

- `sampling_treatment`, because it still clamps to `none`
- favorite and exclusion key-list editors
- direct renderer-lane forcing
- BenchLab forcing controls
- any new preview-only or fullscreen-only divergence controls

These surfaces are not honest enough for this dialog pass and stay out of the
normal UI until a later corrective phase makes them real enough.

## Non-Claim

`U03` is a UI and surface-truth redesign.

It does not claim:

- broad output or treatment rewiring
- preset or theme retuning
- visual-distinctness proof
- stable-scope widening

Those remain later-phase work.
