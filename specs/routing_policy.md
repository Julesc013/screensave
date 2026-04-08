# Routing Policy Specification

This document is normative.

## Intent

`SX08` makes renderer routing explicit, data-driven, and diagnosable without changing the frozen public renderer ladder or turning private substrate details into public product contracts.

The public ladder remains:

- `gdi`
- `gl11`
- `gl21`
- `gl33`
- `gl46`
- `null`

The internal band model remains private and continues to map:

- `universal -> gdi`
- `compat -> gl11`
- `advanced -> gl21`
- `modern -> gl33`
- `premium -> gl46`

## Live Runtime Truth

The live runtime routing source of truth is the compiled saver module policy.

This rule exists because checked-in `manifest.ini` files are durable repository and package metadata, but they are not the only deployment surface for standalone `.scr` artifacts.
Routing for a running saver must therefore be able to rely on compiled product metadata even when repo-source manifests are not available beside the executable.

The checked-in saver manifest remains the durable text mirror for:

- source review
- Suite catalog and browser surfaces
- packaging manifests and copied bundle manifests
- SDK and contributor guidance
- validation and audit surfaces

Those text manifests must stay aligned with the compiled saver policy.

## Saver Policy Model

Each saver now carries a bounded routing policy consisting of:

- supported public renderer kinds, expressed through stable saver capability flags
- `minimum_kind`
- `preferred_kind`
- `quality_class`

Rules:

- `gdi` support remains mandatory.
- support claims must remain contiguous up the public ladder:
  `gl46` implies `gl33`, `gl21`, and `gl11`
  `gl33` implies `gl21` and `gl11`
  `gl21` implies `gl11`
- `minimum_kind` and `preferred_kind` must be supported by that saver.
- `minimum_kind` must not rank above `preferred_kind`.
- `null` remains internal only and is not a saver capability target.

## Text Metadata Model

Saver manifests may carry a `[routing]` section with:

- `minimum_kind`
- `preferred_kind`
- `quality_class`
- optional `degraded_behavior`
- optional `unsupported_paths`

Preset export/import files may carry an optional `[routing]` section with the same keys.

Pack manifests may carry an optional `[routing]` section with the same keys.

These text fields are:

- descriptive and policy-oriented
- limited to durable public-ladder terms
- not a place for backend-private implementation trivia

At `SX08`, preset and pack routing metadata participates in schema validation, import/export, pack discovery, and catalog-facing surfaces.
Live renderer creation still anchors on the compiled saver policy unless a later runtime surface promotes a specific preset or pack artifact into an active routing input.

## Defaults And Migration

Conservative defaulting rules:

- missing saver-manifest routing fields default to the compiled saver policy
- missing preset routing fields mean "no extra routing preference; inherit saver policy"
- missing pack routing fields mean "no extra routing preference; inherit saver policy"
- older preset and pack files without `[routing]` remain valid
- invalid routing metadata must be rejected safely rather than partially applied silently

`SX08` does not require exhaustive annotation of every historical preset or pack.
Representative adoption is sufficient so long as the schema, parsers, and defaults are real.

## Central Evaluation Model

Routing now evaluates these concepts explicitly:

1. the user or harness requested renderer kind
2. the saver's declared minimum and preferred policy
3. the saver's supported public ladder ceiling
4. the effective requested kind after policy clamping or raising
5. observed backend availability and creation success
6. the selected active renderer kind
7. any degraded result and the fallback cause

Rules:

- `auto` means "prefer the saver-declared lane", not "always chase the highest possible lane"
- an explicit request above the saver's supported ceiling must clamp downward honestly
- an explicit request below the saver's supported floor must raise upward honestly
- backend creation failure must still walk down the canonical fallback ladder
- routing must remain truthful even when a saver prefers a lower lane than the highest lane it can optionally exercise

## Diagnostics

Routing outcomes must remain observable through existing diagnostics surfaces.

At minimum, the runtime must be able to surface:

- requested renderer
- saver routing profile
- selection path
- fallback cause
- active renderer

Products remain insulated from private substrate types.
Diagnostics may expose the results, but diagnostics text does not redefine the public renderer doctrine.
