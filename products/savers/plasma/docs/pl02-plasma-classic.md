# Plasma PL02: Plasma Classic Preservation

Status: behavior-preserving classic-surface freeze after `PL01`.

## Purpose

`PL02` makes the current real Plasma product explicit as `Plasma Classic`.
It preserves the current narrow saver surface so later `PL03+` work can change internals without erasing current identity, keys, aliases, routing truth, or import/export expectations.

## Relationship To PL00 And PL01

- [`pl00-constitution.md`](./pl00-constitution.md) froze the rule that the current saver must survive as `Plasma Classic`.
- [`pl01-taxonomy.md`](./pl01-taxonomy.md) and [`pl01-schema.md`](./pl01-schema.md) froze future vocabulary and logical schema direction.
- `PL02` preserves the current repo-supported slice that those later phases must continue to honor.

`PL02` does not broaden Plasma into the larger flagship shape described by `PL00` and `PL01`.

## Why Plasma Classic Exists

Future flagship Plasma is expected to grow.
That future growth must not erase the current saver that already exists in the repo and already ships truthful behavior on the current lower-band posture.

`Plasma Classic` exists to give later work a crisp preserved subset:

- current classic effect families
- current classic preset inventory
- current classic theme inventory
- current classic aliases
- current shared import/export expectations
- current truthful `gdi` floor and `gl11` preferred lane

## Current Repo Reality Being Preserved

The preserved current reality is:

- the saver product is still `Plasma`
- the product remains a compact framebuffer-and-palette saver
- the product remains focused on plasma, fire, and interference-wave motion
- the product remains curated rather than becoming a general effects lab
- the current default preset remains `plasma_lava`
- the current default theme remains `plasma_lava`
- the current routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- the current core identity remains truthful on the `gdi` floor and conservative on the `gl11` lane

`Plasma Classic` is therefore a preserved compatibility and content slice inside future Plasma.
It is not a second standalone saver and it is not a renamed product line.

## Classic Effect-Family Inventory

The current classic effect-family inventory is fixed as:

| Enum | Classic label | Notes |
| --- | --- | --- |
| `PLASMA_EFFECT_FIRE` | `fire` | warm upward-biased classic field mode |
| `PLASMA_EFFECT_PLASMA` | `plasma` | classic multi-source plasma field mode |
| `PLASMA_EFFECT_INTERFERENCE` | `interference` | classic interference-wave mode |

These are the current classic effect families that later phases must preserve materially even if internal implementation changes.

## Classic Preset Inventory

The preserved classic preset keys are:

| Preset key | Theme key | Notes |
| --- | --- | --- |
| `plasma_lava` | `plasma_lava` | default classic warm plasma/fire identity |
| `aurora_plasma` | `aurora_cool` | cool high-detail classic plasma |
| `ocean_interference` | `oceanic_blue` | blue interference classic |
| `museum_phosphor` | `museum_phosphor` | quiet monochrome classic |
| `quiet_darkroom` | `quiet_darkroom` | subdued dark-room classic |
| `midnight_interference` | `midnight_interference` | darker interference classic |
| `amber_terminal` | `amber_terminal` | amber monochrome classic |

Current classic preset count: `7`.

## Classic Theme Inventory

The preserved classic theme keys are:

| Theme key | Notes |
| --- | --- |
| `plasma_lava` | warm lava palette |
| `aurora_cool` | cool aurora palette |
| `oceanic_blue` | deep blue palette |
| `museum_phosphor` | restrained green phosphor palette |
| `quiet_darkroom` | muted dark-room palette |
| `midnight_interference` | deep blue-black interference palette |
| `amber_terminal` | warm amber monochrome palette |

Current classic theme count: `7`.

## Classic Alias / Canonical-Key Policy

The current preserved classic alias set is intentionally small.

| Alias | Canonical key | Applies to | Rule |
| --- | --- | --- | --- |
| `ember_lava` | `plasma_lava` | preset and theme lookup | remains a valid compatibility alias |

Policy:

- current canonical keys stay canonical
- existing aliases remain resolvable
- no new PL02 alias expansion is implied
- future phases may add new content, but they must not silently break the preserved classic canonical keys or alias resolution already present

## Classic Pack Compatibility Notes

The current shipped classic-adjacent pack surface includes:

- pack key `lava_remix`
- built-in `pack.ini`
- built-in preset file `presets/lava_remix.preset.ini`
- built-in theme file `themes/lava_remix.theme.ini`

Preservation rules:

- the pack remains part of the preserved current Plasma content surface
- its current `plasma` product ownership remains unchanged
- its current routing notes remain descriptive rather than runtime-backend doctrine
- future internal migration must continue to represent the pack honestly even if the implementation beneath pack loading later changes

## Truthful Current Band / Routing Posture

The preserved current routing posture is:

- saver product key remains `plasma`
- manifest and compiled policy remain aligned
- `minimum_kind=gdi`
- `preferred_kind=gl11`
- `quality_class=safe`

Current lower-band truth to preserve:

- honest usable `gdi` floor
- conservative `gl11` preferred lane
- no requirement that current classic identity depend on `gl21+`
- preview-safe and long-run-stable current saver posture

`PL02` does not reopen renderer doctrine and does not promote higher-band Plasma behavior into the current classic baseline.

## Plasma Classic Stays Inside Future Plasma

Explicit rule:

- the public saver product remains `Plasma`
- `Plasma Classic` is the preserved compatibility/content slice inside that product's future program
- `Plasma Classic` is not spun out into a separate saver
- later flagship work may wrap or extend Plasma Classic, but it must not replace it dishonestly

## Non-Goals For PL02

`PL02` does not own:

- engine refactor
- runtime-plan compiler work
- new effect families
- new output families
- new treatment systems
- new presentation modes
- transitions or journeys implementation
- settings-surface implementation
- BenchLab implementation
- `gl21` / `gl33` / `gl46` uplift work
- content-system expansion
- set or favorites engine work

## What PL03 May Change Vs What It Must Preserve

`PL03` may change:

- internal data layout
- internal helper structure
- internal field/update organization
- how future non-classic content is represented internally
- how future flagship Plasma composes broader behaviors around the classic slice

`PL03` must preserve:

- product identity `Plasma`
- `Plasma Classic` as an explicit preserved compatibility slice
- current default preset/theme
- current classic effect families
- current classic preset keys
- current classic theme keys
- `ember_lava -> plasma_lava` alias resolution
- current import/export compatibility expectations for classic content
- current honest lower-band routing posture for the classic slice

