# PX41 Community And Curation

## Purpose

This note records the bounded local provenance and curation surfaces landed by `PX41`.

## Actual Community, Provenance, And Curation Surfaces Landed

`PX41` adds two product-local file-first surfaces:

- `products/savers/plasma/curation/curated_collections.ini`
- `products/savers/plasma/curation/provenance_index.ini`

It also adds the report surface:

- `python tools/scripts/plasma_lab.py curation-report`

The curated collections currently admitted are:

- `classic_core_featured`
- `dark_room_support`
- `wave3_experimental_sampler`

The provenance index currently admits one built-in pack entry:

- `lava_remix_builtin`

## Local Ratings, Curated Pools, And Provenance Semantics

The truthful current curation surface is:

- curated stable collections for the preserved classic core and the quieter dark-room support slice
- one opt-in experimental sampler collection for the bounded Wave 3 studies
- one product-local provenance index entry that mirrors the shipped `lava_remix` pack provenance sidecar and adds a local trust-tier label

The truthful current provenance semantics are:

- `channel`
- `support_tier`
- `source_kind`
- `provenance_kind`
- `trust_tier`

`trust_tier` remains product-local vocabulary here.
It is not shared system law.

## What Remains Unsupported Or Intentionally Absent

`PX41` does not create:

- a full community platform
- online collection exchange
- ratings or dislikes
- shared provenance or trust law
- broad portable or user-root provenance discovery

Only the single built-in `lava_remix` pack currently participates in the provenance index.

## Scope Boundary

`PX41` does not create a full community platform.
It adds a bounded local curation and provenance layer that later support or release-review work can inspect honestly.
