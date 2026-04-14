# Plasma U01 Classic Compatibility

## Purpose

This note records what `U01` deliberately preserves while removing `Plasma
Classic` as a runtime regime.

The goal is continuity without architectural dishonesty.

## Compatibility Rule

After `U01`, Classic remains:

- a preserved content family
- the stable default lineage
- a key and alias compatibility surface
- a vocabulary used by curated sets, journeys, packs, and proof history

Classic does not remain:

- a dedicated execution mode
- a plan-compilation switch
- a validation gate
- a second runtime story inside `Plasma`

## Preserved Content And Key Surfaces

`U01` keeps these surfaces intentionally stable:

- default preset key `plasma_lava`
- default theme key `plasma_lava`
- `ember_lava -> plasma_lava` canonical-key migration
- built-in Classic-content sets such as `classic_core`,
  `dark_room_classics`, `warm_classics`, and `cool_classics`
- journey identity such as `classic_cycle`
- pack and provenance surfaces that explicitly preserve Classic identity

## Preserved Helper And Lookup Surfaces

`U01` introduces neutral canonical helpers for the unified runtime, but it
retains the older Classic-named helper entry points as compatibility wrappers.

That means older callers may still resolve:

- `plasma_classic_canonical_key(...)`
- `plasma_classic_preset_count()`
- `plasma_classic_theme_count()`
- `plasma_classic_is_known_preset_key(...)`
- `plasma_classic_is_known_theme_key(...)`

The canonical runtime now prefers the neutral helpers first.

## Preserved Validation And Proof Vocabulary

The canonical validation catalog now describes the preserved default stable
path directly.

Older proof and tooling callers may still resolve the legacy lookup keys:

- `classic_default` -> `default_stable_path`
- `classic_gdi` -> `default_path_gdi`
- `classic_gl11` -> `default_path_gl11`

Those legacy names survive only as lookup aliases for older proof consumers.
They are no longer the canonical product truth.

## Compatibility Caveats

`U01` does not preserve any guarantee that the word `classic` means a separate
engine.

Where Classic wording still appears after this phase, it should be read as one
of:

- preserved content identity
- curated default-family lineage
- historical proof vocabulary
- compatibility aliasing

## Scope Boundary

This note does not claim that settings authority, UI truth, or broader visual
grammar issues are solved.

Those remain later corrective work.
