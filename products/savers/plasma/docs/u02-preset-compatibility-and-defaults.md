# Plasma U02 Preset Compatibility And Defaults

## Purpose

This note records how presets behave after the `U02` settings-authority rewrite.

The goal is simple:
presets stay real, preserved, and compatible, but they stop acting like hidden
runtime masters.

## How Presets Behave After U02

After `U02`, a preset is a curated defaults bundle plus a content identity.

Selecting or loading a preset still provides:

- preset identity and compatibility key
- default theme pairing
- bundled generator, output, treatment, presentation, speed, resolution, and
  smoothing defaults
- deterministic-seed defaults where the preset carries them

Those defaults seed config state.
They do not bypass the later resolved-settings and plan-binding stages.

## What Presets Still Provide

Presets still provide meaningful authored value:

- curated baseline looks such as `plasma_lava`
- stable versus experimental catalog membership
- theme pairing defaults
- bundled grammar defaults for first-load and preset-switch behavior
- import or export continuity for legacy preset-driven config

This keeps the product identity intact.

## What Presets No Longer Own

After `U02`, presets no longer own the final compiled runtime grammar by
themselves.

They no longer get the last word over explicit:

- persisted visual-grammar overrides
- imported config overrides
- session-local settings overrides
- canonical resolved settings state
- requested-versus-resolved reporting

The runtime plan now treats the preset bundle as an earlier layer, not a hidden
second authority path.

## Preserved Compatibility Surfaces

`U02` intentionally preserves all of the following:

- product name `Plasma`
- `Plasma Classic` as content identity and compatibility surface
- default stable baseline `plasma_lava`
- default stable theme `plasma_lava`
- `ember_lava -> plasma_lava` canonical alias behavior
- built-in preset and theme registry entries
- existing config import or export keys for preset and theme identity

No broad preset-schema migration is introduced here.

## Compatibility Caveats

Current bounded caveats still matter:

- selecting a preset in the config dialog intentionally reapplies that preset's
  curated bundle before later explicit overrides are read or changed
- registry load also reapplies the active preset bundle before persisted field
  overrides are layered back on top
- BenchLab `/plasma-preset:` forcing currently changes the selected preset key
  for session identity and reporting, but it does not re-import the full preset
  bundle into config state

Those caveats are explicit now.
They are no longer hidden behind vague preset-authority language.

## Scope Boundary

Presets remain curated defaults and compatibility surfaces.

They are not separate runtime modes, and after `U02` they are not the final
runtime grammar authority either.
