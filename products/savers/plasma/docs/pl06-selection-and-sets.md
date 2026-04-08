# PL06 Selection And Sets

## Purpose

`PL06` makes Plasma's content selection state explicit.
It replaces scattered preset/theme assumptions with a product-local selection model that can resolve:

- selected preset
- selected theme
- active preset set
- active theme set
- stable versus experimental filtering
- favorites-only posture
- exclusions

This is the selection seam later settings, BenchLab, and transition phases will build on.

## Selected State Model

The PL06 selection layer lives in `plasma_selection.h/.c`.
It separates:

- selection preferences:
  - product-local requested state that can be persisted
- resolved selection state:
  - the canonical preset/theme/set result actually used by plan compilation

Current persisted preference fields are:

- `content_filter`
- `favorites_only`
- `preset_set_key`
- `theme_set_key`
- `favorite_preset_keys`
- `excluded_preset_keys`
- `favorite_theme_keys`
- `excluded_theme_keys`

The compiled selection state resolves those preferences into:

- `selected_preset`
- `selected_theme`
- `active_preset_set`
- `active_theme_set`
- resolved favorite and exclusion masks
- whether favorites-only was requested
- whether favorites-only was actually applied after checking the filtered pool

## Default Selection Posture

The current default selection posture remains conservative and honest:

- selected preset: `plasma_lava`
- selected theme: `plasma_lava`
- content filter: `stable_only`
- favorites-only: off
- active preset set: none
- active theme set: none
- favorite and exclusion lists: `none`

That preserves the existing Plasma Classic baseline instead of silently changing the default experience.

## Favorites And Exclusions

Favorites and exclusions are now first-class Plasma product concepts.

Implemented now:

- favorite presets
- favorite themes
- excluded presets
- excluded themes
- product-local persistence for those lists in Plasma config
- canonicalization of aliases and unknown keys during clamp

Current behavior:

- unknown keys are dropped
- aliases are normalized to canonical keys
- exclusions win over favorites
- favorites-only is applied only when the filtered pool actually contains favorite candidates
- if favorites-only would otherwise produce an empty pool, selection falls back honestly instead of failing silently

What is not claimed yet:

- a new public dialog for editing favorites and exclusions
- suite-wide favorites law shared across all savers

## Preset-Set Model

Preset sets are now real product-local concepts.
Each set carries:

- stable set key
- display name
- summary
- channel scope
- weighted inclusion list

Implemented now:

- `classic_core`
  - all preserved built-in classic presets
- `dark_room_classics`
  - `quiet_darkroom`
  - `museum_phosphor`
  - `midnight_interference`
  - `amber_terminal`

Current selection behavior:

- if the requested preset is outside the active set, selection falls back to the first valid set member
- exclusions and favorites can further narrow the active set

## Theme-Set Model

Theme sets are likewise real product-local concepts.
Each set carries:

- stable set key
- display name
- summary
- channel scope
- weighted inclusion list

Implemented now:

- `classic_core`
  - all preserved built-in classic themes
- `dark_room_classics`
  - `museum_phosphor`
  - `quiet_darkroom`
  - `midnight_interference`
  - `amber_terminal`

Theme resolution remains honest to current Plasma behavior:

- the requested theme is preferred if it survives the current filters
- otherwise the resolver first tries the selected preset's canonical theme
- otherwise it falls back to the first valid theme in the active set or registry

## Stable Versus Experimental Pool Behavior

The selection layer understands three product-local filter postures:

- `stable_only`
- `stable_and_experimental`
- `experimental_only`

Implemented now:

- `stable_only` and `stable_and_experimental` both resolve against the current stable pool
- `experimental_only` is supported as a real preference value
- because the current repo does not ship experimental Plasma content yet, `experimental_only` clamps back to `stable_only`

This is deliberate.
PL06 creates the real selection behavior without inventing fake experimental content.

## Interaction With The Shared Randomization Contract

PL06 does not rewrite the shared randomization contract in `screensave_common_config`.

Current behavior:

- shared randomization mode and scope remain the outer contract
- Plasma's product-local selection preferences persist separately from those shared fields
- the current product-specific randomization hook still randomizes effect, speed, resolution, and smoothing only
- it does not silently mutate favorites, exclusions, or active set keys

That keeps PL06 compatible with the current shared contract while creating a clean product-local seam for later selection-aware randomization work.

## Pack Compatibility Notes

The content system preserves the current pack surface:

- `lava_remix` remains a valid shipped pack identity
- its preset and theme asset links remain represented by canonical keys in the content registry
- current import/export and pack-manifest expectations remain intact

PL06 does not yet expose pack selection as a separate public control surface.

## Implemented Now Versus Prepared For Later

Implemented now:

- registry-backed preset/theme/pack/set lookup
- product-local selection preferences and resolved selection state
- product-local favorites and exclusions
- stable-versus-experimental filter handling
- product-local persistence and preset import/export entries for selection metadata
- deterministic fallback rules when requested content falls outside the active pool

Prepared for later, but not claimed as working now:

- transition-set and journey-set execution
- morph compatibility routing
- public settings surfaces for sets and favorites
- BenchLab inspection and forcing surfaces
- broader pack-discovery and user-content workflows

## Scope Boundary

Real morph and journey runtime behavior remains later work.
`PL06` only establishes the product-local selection and set system that those later phases can build on.
