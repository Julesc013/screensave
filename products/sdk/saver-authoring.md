# Saver Authoring

This guide describes the supported path for adding a new first-class saver product in-repo.

## Starting Point

1. Copy `products/savers/_template/` to `products/savers/<slug>/`.
2. Rename every `template_saver_*` source and resource file to `<slug>_*`.
3. Replace manifest keys, display names, preset keys, theme keys, registry roots, and README text in one pass.

Use a canonical saver slug.
Prefer one word unless a later roadmap decision explicitly says otherwise.

## What Lives In The Saver Tree

Keep these in the owning saver subtree:

- manifest identity
- product config enums and defaults
- preset descriptors and preset values
- theme descriptors and colors
- built-in packs
- smoke checks
- saver README and notes

Do not move product-local meaning into `platform/` until at least two real products need the same stable abstraction.

## Minimum Product Shape

A new saver should mirror the `_template` tree:

- `manifest.ini`
- `README.md`
- `presets/defaults.ini`
- `presets/themed.ini`
- `presets/performance.ini`
- `src/<slug>_entry.c`
- `src/<slug>_internal.h`
- `src/<slug>_module.c`
- `src/<slug>_config.c`
- `src/<slug>_presets.c`
- `src/<slug>_themes.c`
- `src/<slug>_sim.c`
- `src/<slug>_render.c`
- `src/<slug>_config.rc`
- `src/<slug>_resource.h`
- `src/<slug>_version.rc`
- `tests/README.md`
- `tests/smoke.c`

## Shared Versus Product-Local Rules

Shared:

- saver lifecycle contracts
- common config fields and randomization outer contract
- renderer selection and fallback
- pack manifest format

Product-local:

- animation behavior
- config meaning
- curated presets
- themes and colors
- content packs

## Renderer Expectations

- Always define a truthful `gdi` path.
- Treat `gl11` and `gl21` as optional.
- Do not claim `gl33` or `gl46` support unless a real backend path exists for the product.
- Do not assume the meta-saver, suite app, or packaging flow will hide unsupported renderer choices.

## Validation Before Build Wiring

Run:

- `python tools/scripts/check_sdk_surface.py <path-to-new-saver-root>`
- the saver's local smoke check when it is build-integrated

Then update role docs, validation notes, and build files together.

## Do Not

- do not add a runtime DLL/plugin loader
- do not add a new saver by editing only build files and skipping manifests/tests/docs
- do not use BenchLab or `suite` as an authoring surface
- do not let one experimental saver redefine the platform contract
