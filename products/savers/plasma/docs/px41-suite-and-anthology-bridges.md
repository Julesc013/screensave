# PX41 Suite And Anthology Bridges

## Purpose

This note records the bounded Plasma-local suite and `anthology` bridge surfaces landed by `PX41`.

## Why PX41 Exists Now

`PX40` finished the first serious CLI-first Plasma Lab and ops toolchain.
`SY40` then froze only a thin shared vocabulary layer for saver versus `anthology` versus `suite` role boundaries and for SDK/reference exposure.
`PX41` uses that thin shared vocabulary to expose Plasma-owned meaning more clearly without moving actual product behavior into shared scope.

## Relationship To SY40, PX40, And PX50

- `PX40` made the report-first and validation-first toolchain real enough to inspect local bridge metadata safely.
- `SY40` promoted only shared role and exposure vocabulary, not shared runtime behavior.
- `PX50` may later use these local bridge surfaces when deciding whether any of them deserve stable release-facing exposure.

## Actual Plasma-Local Suite And Anthology Bridge Surfaces Landed

`PX41` adds the file-first local bridge surface at:

- `products/savers/plasma/integration/projection_surface.ini`
- `python tools/scripts/plasma_lab.py integration-report`

The current projection surface defines four bounded bridges:

- `anthology_stable_core`
- `anthology_experimental_studies`
- `suite_stable_dark_room`
- `suite_experimental_sampler`

Each bridge records:

- the consumer: `anthology` or `suite`
- the visibility class: `stable` or `experimental`
- the product-owned curated collection being exposed
- whether the collection is `featured` and/or `curated`
- the explicit local-only treatment for favorites and exclusions
- journey behavior
- settings handoff and preview visibility where that is relevant to `suite`

## What Shared Contracts Were Consumed

`PX41` consumes the thin shared vocabulary from:

- [`../../../../docs/roadmap/sy40-suite-and-anthology-contract.md`](../../../../docs/roadmap/sy40-suite-and-anthology-contract.md)

That shared contract is vocabulary-only.
It does not define how Plasma favorites, exclusions, weighting, journeys, or curation behave at runtime.

## What Anthology And Suite Can Actually Consume Now

The truthful current consumer-facing surface is still report-first and metadata-first:

- `anthology` can consume which Plasma collections are intentionally exposed, and whether that exposure is `stable` or `experimental`
- `suite` can consume the same visibility split plus bounded local hints about product-owned settings handoff and preview visibility
- both consumers can see whether a bridge is `featured` or `curated`

The current local collections behind those bridges are:

- `classic_core_featured`
- `dark_room_support`
- `wave3_experimental_sampler`

## What Remains Product-Local Or Unsupported

The following remain explicitly product-local:

- how favorites and exclusions affect actual selection
- how journeys are stepped
- how curated collections are chosen or ranked
- how `anthology` or `suite` might actually consume these metadata files in later work

The following remain unsupported:

- cross-product favorites synchronization
- shared weighting or ranking semantics
- automatic suite-side import of every product-local set or journey
- any implication that experimental collections should be shown by default

## Scope Boundary

`PX41` does not redefine suite law.
It exposes product-owned Plasma meaning more clearly through local metadata and local reports only.
