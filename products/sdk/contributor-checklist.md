# Contributor Checklist

Use this checklist before treating a new saver or pack as review-ready.

## New Saver

- Copy `products/savers/_template/` instead of inventing a new tree from scratch.
- Replace every `template_saver` token consistently.
- Keep product logic in the saver tree and shared logic in `platform/` only when reuse is proven.
- Make capability claims honest, especially around `gdi`, `gl11`, and `gl21`.
- Add or update local smoke coverage.
- Run `python tools/scripts/check_sdk_surface.py <path-to-new-saver-root>`.

## New Pack / Preset / Theme

- Keep pack files human-readable and versioned.
- Use canonical saver keys.
- Keep entry paths relative and safe.
- Run `python tools/scripts/check_sdk_surface.py <path-to-pack-root>`.

## Role Boundaries

- Standalone savers are products.
- `anthology` is the suite meta-saver.
- `suite` is the browser/launcher/config surface.
- BenchLab is diagnostics-only.
- SDK material is authoring support, not runtime extension loading.

## Not Baseline

- runtime DLL or plugin loading
- marketplace or remote fetch workflows
- suite-driven authoring
- silent compatibility-baseline changes
