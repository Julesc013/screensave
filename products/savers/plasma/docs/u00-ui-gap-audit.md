# Plasma U00 UI Gap Audit

## Current Dialog Truth

The current dialog is layered, but it is not yet aligned with the actual runtime grammar.

### Basic

Current controls:

- preset
- theme
- speed

Missing from the intended truthful basic surface:

- explicit stable-vs-experimental pool control
- transitions on/off
- a simple visual-intensity entry point

### Advanced

Current controls:

- detail level
- effect family
- resolution
- smoothing
- preset set
- theme set
- transitions enabled
- transition policy
- deterministic seed mode

Mismatch:

- preset set and theme set are not normal advanced controls; they are authoring and curation controls
- output family, output mode, treatments, and presentation are absent even though the runtime and docs discuss them
- deterministic seed mode exists, but the rest of the real grammar is still preset-hidden

### Author/Lab

Current controls:

- content filter
- favorites only
- journey
- transition fallback
- transition seed continuity
- transition interval
- transition duration
- deterministic seed value
- diagnostics overlay

Mismatch:

- content filter is a day-to-day product truth and belongs much higher than Author/Lab
- favorites-only exists without in-dialog favorite or exclusion editing
- diagnostics exist, but the dialog does not surface the rest of the product-local truth it claims

## Current Misleading Behaviors

- Selecting a preset silently rewrites effect, speed, resolution, and smoothing, but also continues to hide the real output/treatment/presentation authority.
- The UI never tells the user that most of the visible experimental grammar is preset-owned.
- BenchLab can see more of the real runtime plan than the product dialog can request.

## Correction Goal

The dialog needs to become a truthful layered control surface:

- Basic for day-to-day preset, theme, pool, motion, and simple safety controls
- Advanced for the real visual grammar
- Author/Lab for curation, journeys, deterministic seed value, diagnostics, and bounded fallback policy
