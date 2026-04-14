# Plasma U06 Preset And Theme Tuning Notes

## Purpose

This note records the first salvage retune of Plasma's built-in presets and themes.

## Theme Retune

All built-in themes were retuned to separate the product families more clearly:

- `plasma_lava`: deeper reds with brighter amber lift
- `aurora_cool`: colder cyan foundations with brighter mint energy
- `oceanic_blue`: deeper blue body with glacial crest highlights
- `museum_phosphor`: restrained green phosphor tuned for long-run monochrome use
- `quiet_darkroom`: darker cinder warmth with lower-contrast highlights
- `midnight_interference`: colder blue-black interference tones
- `amber_terminal`: warmer amber phosphor with brighter terminal highlights

The retune deliberately increases separation between the green and amber monochrome families and between the cool blue families.

## Stable Preset Retune

The stable classic slice now keeps the same keys and preserved content identity while using more truthful lower-band-safe grammar differences:

- `plasma_lava`: preserved as the compatibility and lower-band baseline
- `aurora_plasma`: adds `glow_edge`
- `ocean_interference`: adds `accent_pass`
- `museum_phosphor`: adds `phosphor`
- `quiet_darkroom`: adds `crt`
- `midnight_interference`: adds `emboss_edge`
- `amber_terminal`: adds `phosphor` plus `accent_pass`

This keeps the stable lane narrow while avoiding the earlier collapse into almost-all-raster-none-none-none-flat variants.

## Experimental Retune

The experimental slice keeps its broader grammar role, but the closest overlaps were separated further:

- `aurora_bands`: now uses `glow_edge` instead of accent-only lift
- `emboss_current`: now combines embossed relief with restrained accent lift
- `quasi_crystal_bands`: now combines embossed strata with accent lift

The goal is not to multiply feature families.
It is to make the existing families read differently enough to justify their continued presence.

## Preview, Fullscreen, And Long-Run Intent

This retune favors:

- stronger first-read distinctness in preview windows
- calmer long-run stable presets for dark-room or museum use
- lower-band-safe treatments for the stable slice
- explicit lane-aware degrade behavior instead of hidden richer-lane assumptions
