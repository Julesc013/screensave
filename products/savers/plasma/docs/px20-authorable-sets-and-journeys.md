# PX20 Authorable Sets And Journeys

## Purpose

This note records the first real authored set and journey substrate landed by `PX20`.

## Directories And File Shapes

`PX20` adds these product-local directories:

- `products/savers/plasma/preset_sets/`
- `products/savers/plasma/theme_sets/`
- `products/savers/plasma/journeys/`

Current file kinds:

- `.presetset.ini`
- `.themeset.ini`
- `.journey.ini`

Each file carries:

- a product-local `kind`
- `version=1`
- `product.key=plasma`
- `schema_version=1`

## Implemented Preset-Set Surface

Current authored preset sets are:

- `classic_core`
- `dark_room_classics`
- `fire_classics`
- `plasma_classics`
- `interference_classics`
- `warm_bridge_classics`
- `cool_bridge_classics`

Each preset-set file makes these semantics explicit:

- set key
- display text
- summary
- `profile_scope`
- weighted ordered members

## Implemented Theme-Set Surface

Current authored theme sets are:

- `classic_core`
- `dark_room_classics`
- `warm_classics`
- `cool_classics`

Theme-set files use the same bounded semantics:

- set key
- display text
- summary
- `profile_scope`
- weighted ordered members

## Implemented Journey Surface

Current authored journeys are:

- `classic_cycle`
- `dark_room_cycle`
- `warm_bridge_cycle`
- `cool_bridge_cycle`

Current journey semantics are intentionally small:

- ordered step list
- preset-set reference
- theme-set reference
- `policy=preset_set`
- positive `dwell_millis`
- `journey_intent=ordered_cycle`

## What Is Real Now Versus Later

Real now:

- checked-in authored set and journey files
- loader and validator support for those files
- parity checks against the compiled Plasma set and journey surface

Later:

- broader file kinds
- richer author constraints
- portable or user-authored discovery roots
- larger authoring workflows

## Current Limitations

- built-in preset and theme descriptor inventories still remain primarily compiled and legacy-INI anchored
- journeys currently expose only the bounded ordered-cycle subset
- no user-facing editing surface writes these files yet
- no broader migration tool exists yet
