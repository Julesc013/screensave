# SY40 SDK Reference Contract

## Purpose

This document freezes the minimum shared Wave 4 SDK and reference-facing contract that is genuinely shared now.

It covers how saver identity and capability posture are exposed through shared contributor-facing reference surfaces.
It does not create a new shared runtime framework.

## Why This Contract Is Shared Now

This contract is shared now because the repo already has more than one real consumer of the same outer saver metadata:

- saver manifests expose identity, defaults, and capability posture
- `suite` consumes that metadata for browsing and settings handoff
- the SDK documents that metadata for contributor-facing reference and validation

Delaying this thin contract would leave contributor docs and consumer docs free to drift apart on the same outer concepts.

## Shared SDK And Reference Scope

The shared scope in this document is limited to:

- canonical saver identity vocabulary
- default preset and theme exposure vocabulary
- capability and routing posture vocabulary already present in shared outer formats
- stable-versus-experimental visibility wording when a product documents or exports optional richer material
- the distinction between product-owned catalog descriptors and shared outer export or import shells

## Required Shared Concepts

The following concepts are required whenever a product exposes itself through shared SDK or reference surfaces:

| Concept | Shared Meaning |
| --- | --- |
| Canonical saver key | The stable machine identifier for the product |
| Display name | The human-readable product name used in docs and consumer surfaces |
| Summary | Short role-oriented description of the product |
| Default preset key | The default preset the product claims today |
| Default theme key | The default theme the product claims today |
| Capability posture | Honest shared outer claims such as `gdi`, `gl11`, preview safety, long-run stability, `minimum_kind`, `preferred_kind`, and `quality_class` |
| Stable visibility | The subset of documented or exported material that belongs to the product's current stable posture |
| Experimental visibility | The subset that exists but must stay explicitly opt-in and clearly marked |

## Optional Shared Concepts

The following concepts are optional and remain bounded:

| Concept | Shared Meaning |
| --- | --- |
| Preset display name | Human-readable name for a preset when the product provides descriptors |
| Theme display name | Human-readable name for a theme when the product provides descriptors |
| Sample pack | Example pack content that uses the shared outer pack shell while remaining product-owned in meaning |
| Capability notes | Extra honest notes about degraded, bounded, or richer optional lanes |

Optional means a product may omit these when it does not yet support them honestly.

## Shared Reference Boundary

The shared outer shell applies to:

- `manifest.ini`
- exported `.preset.ini` files
- exported `.theme.ini` files
- `pack.ini`
- contributor-facing docs that explain those files

The shared outer shell does not own:

- saver-local preset semantics
- saver-local theme semantics
- saver-local favorites, journeys, or curation
- saver-local provenance, trust, or community metadata
- shared automation or control behavior

## How Products Such As Plasma Should Consume It

Products such as Plasma should:

- keep preset, theme, and content meaning product-owned
- continue to use the shared outer headers and capability posture honestly
- mark richer optional lanes or content slices as experimental when they are not part of the stable baseline
- avoid presenting product-local convenience surfaces as if they were already shared SDK law

## Explicit Statement Of What Remains Product-Local

Automation and control bridge semantics were not promoted by `SY40`.
Community, provenance, and index vocabulary were not promoted by `SY40`.
Favorites, weighting, and curation behavior were not promoted by `SY40`.

Those concerns remain product-local until a later review proves real cross-product need.
