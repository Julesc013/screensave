# Pack Authoring

This guide describes the supported path for shareable data-driven extension content.

## First-Class Extension Path

Packs, presets, and themes are the first-class extension path in the current architecture.
They are data-only.
They do not load code.

Use packs for:

- curated preset bundles
- theme bundles
- scene-data files when the owning saver supports them

Do not use packs to smuggle runtime behavior changes that belong in a real saver product.

## Pack Layout

The canonical built-in pack layout is:

- `products/savers/<slug>/packs/<pack_key>/pack.ini`
- `products/savers/<slug>/packs/<pack_key>/presets/*.preset.ini`
- `products/savers/<slug>/packs/<pack_key>/themes/*.theme.ini`
- optional `scene_*` files when the owning saver documents them

The SDK example pack lives under `products/sdk/examples/template_pack/`.

## Discovery Roots

Current source kinds are:

1. built-in
2. portable
3. user

Current contributor-facing placement rules are:

- built-in authoring lives under `products/savers/<slug>/packs/<pack_key>/`
- portable staging copies packs under `out/portable/<bundle-name>/PACKS/`
- installed mode preserves user-managed config and future user-pack roots outside the managed install tree; do not hardcode install-only assumptions into a pack

The manifest format is already real.
Portable and user directory binding stays distribution-defined and should not be reinvented inside a pack.

## Validation

Run:

- `python tools/scripts/check_sdk_surface.py products/sdk/examples/template_pack`
- `python tools/scripts/check_sdk_surface.py <path-to-pack-root>`

The helper checks:

- required manifest fields
- safe relative entry paths
- presence of referenced preset/theme files
- required preset/theme header sections

## Do Not

- do not add executable code to packs
- do not use absolute paths or `..` path traversal
- do not claim a pack changes renderer capabilities
- do not turn pack authoring into a plugin or marketplace workflow
