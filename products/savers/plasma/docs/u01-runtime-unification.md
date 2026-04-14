# Plasma U01 Runtime Unification

## Purpose

This note records the `U01` runtime-unification cut for Plasma.

`U01` exists to make one thing explicit and enforceable:
`Plasma Classic` is preserved content identity and compatibility surface, not a
separate runtime regime.

This is a runtime-shape correction.
It is not the later settings-authority rewrite.

## Why U01 Exists Now

Older Plasma planning and proof language carried too much Classic-first
architecture vocabulary forward after the actual runtime had already moved
toward one compiled plan.

That created two risks:

- stale code and proof helpers continuing to imply a Classic-engine center
- validation and support surfaces talking as if the preserved default path were
  a separate execution mode

`U01` freezes the product on one runtime story before later settings and UI
work build on it.

## Pre-U01 Runtime Truth

Before this corrective pass, the repo already had material unification work in
place:

- no live `classic_execution` field remained in the compiled plan
- plan compilation already resolved one runtime grammar and then validated it by
  capability and lane
- Classic had already survived mainly as content identity, aliases, curated
  sets, journeys, and historical proof vocabulary

What still remained was residue:

- Classic-named helper entry points still read like the primary runtime surface
- validation and performance catalogs still used `classic_*` names as canonical
  proof keys
- smoke helper naming still implied a Classic-plan regime even when compiling a
  normal direct plan

## Post-U01 Runtime Truth

After `U01`, Plasma has one explicit runtime model:

- one compiled plan
- one validation story
- one lane and degrade model
- one content-selection model

Classic now means:

- preserved default lineage
- preserved content family
- preserved key and alias compatibility
- preserved curated sets, journeys, and pack provenance

Classic no longer means:

- special execution flag
- separate plan mode
- separate validation mode
- separate runtime engine

## What "Classic No Longer As Runtime Regime" Means

Concretely, `U01` now means all of the following:

- canonical runtime helpers use neutral product-wide names first
- canonical validation keys describe the preserved default stable path directly
- legacy `classic_*` helper and validation names survive only as compatibility
  wrappers or lookup aliases
- smoke and proof surfaces no longer need Classic-regime vocabulary to describe
  direct-plan compilation

## What Was Removed Or Simplified

This phase simplified the remaining runtime-facing residue by:

- introducing neutral canonical content, preset, and theme helper entry points
- updating internal content and BenchLab paths to use those canonical helpers
- renaming the smoke helper that compiled a direct plan so it no longer implies
  a Classic-mode build path
- changing the canonical validation matrix and performance envelope keys from
  `classic_*` forms to preserved-default-path forms
- preserving the older `classic_*` names only as compatibility aliases

## What Did Not Change

`U01` intentionally does not change:

- product name `Plasma`
- default preset `plasma_lava`
- default theme `plasma_lava`
- `ember_lava -> plasma_lava` migration behavior
- truthful `gdi` floor
- truthful `gl11` preferred stable lane
- stable versus experimental classification
- requested, resolved, and degraded reporting

## What Remains Deferred To U02+

`U01` does not claim that later corrective work is finished.

It leaves these areas explicitly deferred:

- settings-authority cleanup and preset-bundle minimization
- settings-surface redesign
- output, treatment, and presentation strengthening
- later destructive simplification, retuning, proof expansion, and recut work

## Scope Boundary

`U01` is runtime unification and compatibility preservation.

It should not be read as:

- a settings-authority rewrite
- a UI redesign
- a new feature tranche
- a stable-widening decision
