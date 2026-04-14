# Plasma U00 Diagnostic Baseline

## Purpose

This document freezes the corrective-program baseline for Plasma against the
current repository snapshot.

It is intentionally blunt.
Its job is to describe the product truth that later corrective work must obey,
not to flatter the breadth of the current implementation.

U00 is a diagnostic and control phase.
It does not rewire the runtime, redesign the settings model, retune content, or
recut stable versus experimental posture.

## Why The Corrective Program Exists

Plasma accumulated a broad architecture and a broad documentation stack faster
than it accumulated one coherent product story.

The corrective program exists to stop three failure modes:

- product docs and settings surfaces claiming more authority than the runtime
  really supports
- feature breadth surviving as enum or planning vocabulary after it stopped
  being supportable as a truthful product surface
- stable-facing content and controls drifting back toward crowding, hidden
  authority, and paper distinctions

Repo-truth note:
the current snapshot already contains later salvage work beyond the original U00
problem statement.
That does not make U00 obsolete.
It means U00 now serves as the canonical baseline and anti-regression freeze for
future Plasma work.

## Verified Current Repo Truth

The current snapshot materially verifies these facts:

- product identity remains `Plasma`
- `Plasma Classic` survives as content identity and compatibility surface, not
  as a separate runtime regime
- the stable default center remains `plasma_lava` plus `gdi` floor, `gl11`
  preferred lane, and `quality_class=safe`
- requested, resolved, and degraded truth is explicit in the runtime plan and
  in BenchLab reporting
- generator, output, treatment, and presentation grammar is now settings-owned
  in the compiled plan
- stable versus experimental remains explicit in content and support posture
- the current proof surface includes smoke, BenchLab-facing validation, authored
  substrate validation, and bounded preset-signature auditing

These truths are real in the current repo.
They take precedence over older Plasma docs that describe a broader but less
truthful earlier posture.

## Corrective Assumptions Rechecked

The original salvage assumptions are no longer all fully true in this snapshot.
They now break down as follows.

### `Plasma Classic` is still a runtime regime

False in the current snapshot.

Classic is preserved as identity, defaults, aliases, and compatibility surface.
It is no longer the architectural center of plan validation or execution.

### Many settings still do not materially affect the plan or the image

Partially true.

The main shipped settings surface now drives the compiled plan directly.
The remaining dead or weak surfaces are mostly:

- unsupported or hidden enum values
- conditionally available transition controls
- selection and curation controls whose effect is indirect or long-run
- diagnostics-only controls

### Presets still secretly own too much of the visual grammar

Only partially true now.

Presets still own content identity, default pairings, and curated starting
bundles.
They no longer own final output family, treatment slots, or presentation mode in
the compiled plan.

### Output, treatment, and presentation distinctions are weaker than the docs imply

Still partially true.

The supported surfaced distinctions are real enough to justify their existence.
The remaining weakness is in bounded or unsupported breadth:

- unsupported enums still exist in code vocabulary
- some stable preset pairs remain close
- premium and broader experimental grammar still rely on bounded proof rather
  than broad visual acceptance

### Multiple presets, themes, or combinations are near-duplicates in practice

Still partially true.

The current preset audit no longer reports exact duplicate compiled signatures,
but it still reports close pairs in the stable and experimental sets.

### The settings dialog is misleading or incomplete relative to the real grammar

Partially true.

The dialog is much more truthful than the old pre-U03 surface, but the repo
still carries one large control surface that mixes normal product controls with
authoring and transition controls.
It also still lacks direct favorite and exclusion editing while exposing the
`favorites_only` gate.

### The stable default product collapses into one or two looks

Only partially true now.

The stable family is materially better separated than before, but it remains
intentionally narrow and still contains close pairs that need continued scrutiny.

### The current visual quality is not acceptable

Too broad to state as a current repo truth.

The stable product is improved enough to support the current `GO WITH CAVEATS`
posture.
What remains true is narrower:

- visual distinctness is still bounded
- some breadth is still experimental for good reason
- the proof envelope is still not broad enough to justify relaxed quality claims

## Runtime Truth Versus Doc Truth

The main current divergence is no longer runtime versus code.
It is older docs versus current repo reality.

The current repo has already moved past several old U00 assumptions:

- runtime unification has already happened
- settings authority is already substantially rewritten
- the current dialog already exposes real generator, output, treatment, and
  presentation controls

Older docs that still read as if Classic remains a runtime gate or as if most of
the grammar is still preset-hidden should be treated as historical context, not
current truth.

## Most Serious Current Product-Coherence Failures

The current snapshot is materially more coherent than the earlier Plasma waves,
but these failures still matter:

- unsupported breadth still survives as enum vocabulary and import/export terms
- some stable and experimental preset identities remain too close to rely on
  naming alone
- a few controls remain conditionally real or only indirectly visual, especially
  in transition and selection surfaces
- the dialog still mixes everyday controls and authoring-grade controls in one
  product-facing window
- the proof harness is bounded and does not replace broad screenshot or
  cross-hardware perceptual QA

## U00 Boundary

U00 does not solve those failures.

U00 freezes:

- what is already real
- what is still weak or bounded
- what later phases must preserve
- what later phases must simplify, hide, retune, or prove

Later corrective work may only widen breadth again after it clears those
coherence and proof obligations explicitly.
