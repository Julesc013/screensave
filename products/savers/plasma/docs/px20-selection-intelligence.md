# PX20 Selection Intelligence

## Purpose

This note records the bounded selection-intelligence foundation landed by `PX20`.

## What Becomes More Real In PX20

`PX20` does not invent a new broad selection engine.
It makes the current product-local selection substrate more data-shaped in two concrete ways:

- authored preset-set and theme-set files now hold real weighted membership
- fallback selection now honors those authored weights instead of treating them as decorative data only

## Current Selection Primitives

Plasma already had:

- stable-versus-experimental filtering
- favorites and exclusions
- preset-set and theme-set gating
- journey-aware transition selection

`PX20` strengthens that base by making weighted authored membership part of actual fallback behavior.

## Interaction With Stable And Experimental Pools

- the default product path remains stable-only
- stable default preset and theme remain `plasma_lava`
- no experimental pool is broadened here
- no stable widening is implied here

## What Remains Bounded

The current selection foundation still does not claim:

- persistent anti-repeat memory across sessions or long runs
- semantic similarity reasoning
- rich safety-tag reasoning
- large authored history models

Those remain later-wave selection work.
