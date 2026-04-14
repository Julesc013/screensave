# Plasma U00 Visual Distinctness Audit

## Purpose

This note records the pre-salvage visual distinctness problems that justify the recut.

## Stable Core Presets

The stable classic preset set is real, but several distinctions are thinner than the documentation suggests because they share the same hidden raster-plus-flat grammar and differ mostly by theme, effect, or coarse tuning.

Current examples:

- `plasma_lava` and `quiet_darkroom` share the same hidden `raster` / `native_raster` / `flat` path and differ mostly by theme and resolution pacing
- `museum_phosphor` and `amber_terminal` both read as subdued monochrome plasma variants with the same hidden raster path
- `ocean_interference` and `midnight_interference` are honestly different, but much of the difference still comes from theme and pacing rather than a settings-owned output grammar

## Experimental Breadth

The experimental preset slice does create more visible differences than the stable slice because the preset table is allowed to change output family, treatment slots, and presentation mode.

That breadth is real enough to render different pictures.
It is not real enough to support the current settings-story because:

- the user cannot directly ask for most of those differences
- the preset owns the grammar rather than the settings layer
- stable-versus-experimental truth sits mostly in content metadata rather than in the direct control surface

## Paper Distinctions

The repo still carries multiple documented or enumerated distinctions that do not survive as truthful first-class product surfaces:

- sampling treatments beyond `none`
- raster `dithered_raster`
- surface output family work
- later filter enums that validate as unsupported
- billboard-volume presentation

## Audit Outcome

Pre-salvage Plasma has real visual variety, but that variety is attached to curated preset islands instead of to a coherent, truthful, user-controlled grammar.
