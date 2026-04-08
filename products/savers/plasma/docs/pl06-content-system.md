# PL06 Content System

## Purpose

`PL06` makes Plasma's content surface explicit and product-owned.
It turns the preserved Plasma Classic preset/theme inventory and shipped pack surface into a real internal content registry instead of leaving them as scattered assumptions in config and plan code.

## Relationship To PL00-PL05

- `PL00` froze Plasma's identity, lower-band truth, and product-local metadata direction.
- `PL01` froze the vocabulary for presets, themes, packs, sets, channels, and selection metadata.
- `PL02` froze the preserved Plasma Classic content slice and the keys that must survive.
- `PL03` introduced the `authored content -> compiled plan -> execution state` engine shape.
- `PL04` made output, treatment slots, and presentation explicit runtime stages.
- `PL05` proved the refactored engine still holds an honest `gdi` and `gl11` lower-band baseline.

`PL06` builds on that base by giving Plasma a real product-local content layer without claiming transition, settings-surface, or BenchLab work that belongs to later phases.

## Current Content Reality Before PL06

Before `PL06`, Plasma already shipped real content, but the product surface was still mostly implied:

- built-in presets live in `presets/defaults.ini` and `presets/themed.ini`
- built-in profile-style defaults live in `presets/performance.ini`
- built-in classic preset keys are:
  - `plasma_lava`
  - `aurora_plasma`
  - `ocean_interference`
  - `museum_phosphor`
  - `quiet_darkroom`
  - `midnight_interference`
  - `amber_terminal`
- built-in classic theme keys are:
  - `plasma_lava`
  - `aurora_cool`
  - `oceanic_blue`
  - `museum_phosphor`
  - `quiet_darkroom`
  - `midnight_interference`
  - `amber_terminal`
- the canonical alias `ember_lava -> plasma_lava` already existed
- the shipped `lava_remix` pack already existed as a data-only bundle under `packs/lava_remix/`

That content was real, but it was not yet represented by a dedicated product-local registry and selection layer.

## Target PL06 Shape

`PL06` establishes these product-local internal constructs:

- a content registry for presets, themes, packs, preset sets, and theme sets
- stable lookup by canonical key
- product-local source metadata
- product-local channel metadata
- product-local set membership metadata
- a selection layer that resolves preset/theme choices against filters, favorites, exclusions, and active sets

The content system is internal and real.
It is not yet a public selection UI, transition system, or BenchLab surface.

## Registry Model

The PL06 registry is implemented in `plasma_content.h/.c`.
It currently provides:

- preset entries:
  - canonical preset key
  - pointer to the existing preset descriptor
  - source identity
  - channel identity
- theme entries:
  - canonical theme key
  - pointer to the existing theme descriptor
  - source identity
  - channel identity
- pack entries:
  - pack key
  - display and summary text
  - source identity
  - channel identity
  - routing notes already relevant to the shipped pack surface
  - asset references back to canonical preset/theme keys
- preset set entries:
  - set key
  - display name
  - summary
  - channel scope
  - weighted membership list
- theme set entries:
  - set key
  - display name
  - summary
  - channel scope
  - weighted membership list

The registry is validated at runtime and used by plan compilation and selection resolution.
It is not dead documentation.

## Source Model

The registry carries product-local source identity with these terms:

- `built_in`
- `pack`
- `portable`
- `user`

What is implemented now:

- built-in classic presets and themes are represented as `built_in`
- the shipped `lava_remix` surface is represented as `pack`

What is only prepared for later work:

- dynamic portable discovery
- dynamic user content discovery

PL06 does not pretend those later sources are already discovered automatically just because the vocabulary now exists.

## Stable Versus Experimental Channel Model

The registry carries product-local content-channel identity:

- `stable`
- `experimental`

What is implemented now:

- all shipped classic presets, themes, sets, and the shipped pack are tagged `stable`
- the experimental pool is structurally supported by the registry and selection logic
- the current repo does not fabricate experimental content just to populate that pool

Current honest posture:

- stable content exists and is used
- experimental content is currently empty
- experimental-only selection requests clamp back to the stable pool instead of pretending experimental content exists

## Inventory Now Represented By The Content System

The PL06 registry explicitly represents:

- 7 built-in Plasma Classic presets
- 7 built-in Plasma Classic themes
- 1 shipped pack:
  - `lava_remix`
- 2 product-local preset sets:
  - `classic_core`
  - `dark_room_classics`
- 2 product-local theme sets:
  - `classic_core`
  - `dark_room_classics`

The `classic_core` sets mirror the preserved classic inventory.
The `dark_room_classics` sets provide a smaller stable subset for subdued sessions without changing the default classic baseline.

## Pack Compatibility

`PL06` preserves the shipped pack surface honestly:

- `lava_remix` remains represented as a data-only pack
- its manifest path and asset list still map to the shipped preset/theme files
- the content registry records its pack identity and canonical preset/theme links

What `PL06` does not claim:

- a generic new pack browser UI
- a suite-wide pack law rewrite
- a fully dynamic runtime pack override engine for arbitrary future content directories

## What Remained Intentionally Deferred

`PL06` does not implement:

- morph or journey runtime behavior
- transition-set execution
- new public settings UI for favorites, exclusions, or sets
- BenchLab-facing selection tooling
- advanced-lane or premium-lane content expansion
- shared suite-wide favorites or set schema law

## Current Limits

The PL06 content system is intentionally bounded:

- the registry is explicit over the shipped Plasma surface, not an unlimited discovery engine
- product-local sets are implemented in code, not as a shared suite-wide file format
- favorites and exclusions are persisted product-locally in Plasma config, but there is no new public UI for them yet
- transition and journey metadata remain later work

## What PL07 May Build On Next

Later phases can now build on a real content layer instead of ad hoc preset/theme assumptions.
The next phases may extend:

- richer channel splits
- more content families
- broader pack discovery
- transition compatibility metadata
- settings and BenchLab surfaces that expose the content system safely

## Scope Boundary

`PL06` is a product-local content-system phase.
It is not transition work, BenchLab work, settings-surface work, or higher-band feature uplift.
