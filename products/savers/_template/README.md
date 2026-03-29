# Saver Template

Purpose: real contributor template for adding a new first-class saver product in-repo.
Belongs here: copy-forward product files, manifest structure, preset samples, source skeletons, resource templates, and smoke coverage that mirror the live saver architecture.
Does not belong here: shipping product branding, shared platform code, generated build files, or runtime plugin hooks.
Current stage: `C10` turns `_template` into the canonical starter saver surface for contributors.
Type: product template.

## Included Surface

This tree now mirrors the real saver product contract:

- `manifest.ini` for canonical product identity and honest capability flags
- `presets/` with starter preset sets that match the template config enums
- `src/` with an entry shim, module hooks, config skeleton, preset/theme descriptors, a simple simulation path, a simple renderer path, config resources, and version metadata
- `tests/` with a lightweight smoke check

## How To Use It

1. Copy `_template` to `products/savers/<slug>/`.
2. Rename every `template_saver` file and symbol token to `<slug>`.
3. Replace manifest fields, display text, registry roots, preset keys, and theme keys in one reviewable pass.
4. Run `python tools/scripts/check_sdk_surface.py <path-to-new-saver-root>`.
5. Wire the new saver into build and status surfaces only after its docs, tests, and metadata are coherent.

## Boundaries

- Keep product behavior, presets, themes, and content in the new saver tree.
- Keep shared contracts in `platform/include/screensave/` and shared runtime helpers in `platform/src/core/` only when reuse is proven.
- Keep BenchLab, `anthology`, and `suite` out of the authoring path; they consume saver products, they do not define them.
