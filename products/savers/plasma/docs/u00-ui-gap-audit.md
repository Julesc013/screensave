# Plasma U00 UI Gap Audit

## Purpose

This note records the gap between the current dialog surface and the current
runtime truth.

The current gap is no longer the old pre-U03 problem where the UI could not ask
for most of the real grammar.
The current gap is narrower:

- too many unlike controls still live in one dialog
- some controls are truthful but not equally appropriate for normal users
- some real support and curation controls still lack a matching editing surface
- unsupported vocabulary still exists beneath the dialog and can leak into docs
  or import language if later work is careless

## What The Current UI Exposes

The current dialog exposes three settings surfaces through one product window.

### Basic

- preset
- theme
- speed
- visual intensity
- content pool
- transitions enabled

### Advanced

- generator family
- output family
- output mode
- field resolution
- smoothing
- filter treatment
- emulation treatment
- accent treatment
- presentation mode
- deterministic seed mode

### Author/Lab

- preset set
- theme set
- favorites only
- transition policy
- journey
- transition fallback
- transition seed continuity
- transition interval
- transition duration
- deterministic seed value
- diagnostics overlay

## What The Runtime Grammar Actually Needs

The current runtime needs the following categories to stay explicit:

- content identity and palette
- generator, output, treatment, and presentation grammar
- stable versus experimental pool control
- deterministic seed policy for proof and reproducibility
- transition and journey controls for bounded long-run behavior
- curation and authored-selection controls
- diagnostics-only controls

The main runtime grammar is now represented in the dialog.
The remaining problem is placement and product truth, not total absence.

## Misleading Or Incomplete UI Surfaces

- `transitions_enabled` is a truthful Basic control, but its visible effect is
  conditional and long-run rather than immediate. It needs stronger framing so
  it does not read like a simple always-visible visual toggle.
- `favorites_only` is truthful only when favorite lists exist, but the dialog
  still does not let the user create or edit those lists directly.
- preset and theme sets are real authoring controls, but they remain easy to
  read as ordinary browsing controls even though they mainly exist to drive
  curation and transition behavior.
- the dialog still does not surface requested versus resolved versus degraded
  state directly; BenchLab remains the truthful support surface for that.
- the product window still hosts normal controls and support-grade controls in
  one place. The surface split is real, but the window remains dense.

## Missing Or Intentionally External Controls

These are not missing by accident, but the gap still matters:

- direct favorite and exclusion editing
- direct authored set and journey editing
- direct proof, capture, or BenchLab reporting inside the product dialog
- explicit visual explanation of lane-sensitive degradation for premium
  presentation choices

These are acceptable external or later-phase surfaces for now, but they should
not be mistaken for solved dialog problems.

## Controls That Should Stay Hidden Until Real

- `sampling_treatment`
- unsupported output family and mode vocabulary
- unsupported filter vocabulary
- unsupported presentation vocabulary

If these are surfaced again before they become real, the UI will start lying
again.

## First-Pass Future Split

The current repo already approximates the intended split.
The remaining U03-style anti-regression map should stay:

### Basic

- preset
- theme
- speed
- visual intensity
- content pool
- transitions enabled

### Advanced

- generator family
- output family
- output mode
- resolution
- smoothing
- filter treatment
- emulation treatment
- accent treatment
- presentation mode
- deterministic seed mode

### Author/Lab

- preset set
- theme set
- favorites only
- journey
- fallback policy
- transition seed policy
- transition timing
- deterministic seed value
- diagnostics overlay

## U00 Conclusion

The current UI is no longer fundamentally disconnected from the runtime.
The remaining coherence gap is that it still compresses everyday controls,
conditional transition controls, and authoring-grade curation controls into one
product settings experience.
