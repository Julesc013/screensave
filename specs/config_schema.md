# Configuration Schema Specification

This document is normative.

## Purpose

ScreenSave needs one conceptual configuration model that can serve many savers and suite-level applications without turning every product into a special case.

Series 00 defined the boundary.
`C04` adds the first concrete shared settings, preset/theme file, and pack-manifest rules without redefining product ownership.

## Configuration Layers

Configuration is expected to resolve in layers:

1. Shared platform defaults.
2. Product defaults.
3. Named preset selection.
4. Theme selection.
5. User overrides.
6. Session-only runtime state that is not persisted as stable configuration.

Later implementation series may refine resolution details, but they must preserve the distinction between these layers.

## Shared Taxonomy

The shared contract is intentionally bounded:

- basic settings: stable end-user settings that belong across many savers
- advanced settings: saver-specific settings that remain inside product-owned config
- presets: named shareable behavior bundles
- themes: named shareable appearance bundles
- randomization: within-saver session randomization only
- packs: data-only bundles of preset/theme/scene content
- migration state: explicit schema and format version markers

The current shared common state may include:

- `schema_version`
- `detail_level`
- deterministic seed mode and seed value
- diagnostics-overlay preference
- `randomization_mode`
- `randomization_scope`
- selected `preset_key`
- selected `theme_key`

Anything beyond that default list stays product-local unless at least two products need the same stable meaning.

## Presets

Presets are curated behavior bundles.
They are intended to change how a product behaves, not to identify the product itself.

Presets belong to the owning product unless they are later promoted into a shared suite-level concept.

The baseline shared preset format is a human-readable `.preset.ini` file with these sections:

- `[format]` with `kind=preset` and `version=1`
- `[product]` with the canonical saver `key` and a `schema_version`
- `[common]` for shared settings such as detail level, seed mode, preset key, theme key, and randomization settings
- optional `[product]` keys consumed by the owning saver's import/export hooks

## Themes

Themes are presentation-oriented selections such as palette, mood, or surface treatment.
They should avoid redefining the entire runtime model unless a later spec explicitly broadens their role.

Theme selection should remain separable from preset selection wherever practical.

The baseline shared theme format is a human-readable `.theme.ini` file with these sections:

- `[format]` with `kind=theme` and `version=1`
- `[product]` with the canonical saver `key` and a `schema_version`
- `[theme]` with the owning theme key plus descriptive appearance fields such as display text and primary/accent colors

Theme files remain product-owned even when they use the shared outer header.

## Randomization

`C04` defines within-saver randomization only.
The shared randomization mode is either:

- `off`
- `session`

The shared randomization scope is a bitfield composed only from:

- `preset`
- `theme`
- `detail`
- `product`

Cross-saver orchestration, favorites, and weighting remain deferred to later suite-level work.

## Packs

Packs are shareable data bundles, not code extensions.
The baseline pack manifest is a human-readable `pack.ini` file with:

- `[pack]` containing `format=screensave-pack`, `version=1`, `schema_version`, canonical `product_key`, `pack_key`, `display_name`, and optional description text
- `[files]` containing relative `preset_*`, `theme_*`, and `scene_*` entries

Pack entries must be safe relative paths.
Invalid manifests or invalid entry paths must be rejected without crashing the host and without loading code.

Current pack source kinds are:

- built-in
- portable
- user

When more than one source root is scanned, the discovery order is:

1. built-in
2. portable
3. user

Built-in sample packs live under `products/savers/<slug>/packs/<pack_key>/`.
Portable and user roots remain intentionally data-source categories in `C04`; later Windows-path and distribution work will bind them to concrete external directories without redefining the shared manifest format.

## Defaults

Every product should define a stable default configuration.
That default must be valid on the universal baseline required for that product class.

Defaults should prefer predictable behavior and conservative resource assumptions.

## Persistence Boundaries

Shared configuration should contain only settings that genuinely belong across products or across multiple saver runs.

Shared configuration may include categories such as:

- preferred language-neutral behavior flags
- default duration or density classes
- shared presentation preferences
- platform-level fallback preferences

Product-local configuration should contain:

- product-specific animation parameters
- product-specific content selections
- product-local preset and theme identifiers

Session-only runtime state should not be persisted as durable user configuration unless a later series deliberately promotes it.

## Shared Versus Product-Local Keys

Promotion into shared config requires evidence of reuse and stable meaning across products.
A key must not move into the shared schema merely because one product needs it early.

The default rule is:

- shared when the concept is cross-product and stable
- product-local when the concept is product-specific or experimental

## Versioning And Migration

The shared common state carries a `schema_version`.
Preset/theme export files carry a format `version` plus product `schema_version`.
Pack manifests carry a manifest `version` plus product-facing `schema_version`.

Writers must emit canonical saver keys.
Readers may translate legacy saver keys through the canonical rename map when that is the narrowest safe migration path.
Imported settings must be clamped through the owning saver contract before they are treated as valid runtime state.

Unsupported or invalid preset/theme/pack data must be rejected safely with diagnostics rather than partially applied silently.

## Storage Commitments

The current Windows baseline uses registry-backed persistence for durable shared common state alongside product-owned config persistence.
Import/export uses the versioned text formats above.
Later series may add portable or suite-managed mirrors, but they must preserve the conceptual boundary and versioning rules defined here.
