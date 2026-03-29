# Template Saver

Purpose: contributor-facing map for the real saver template under `products/savers/_template/`.
Belongs here: copy and rename guidance, integration notes, and validation reminders for first-class saver authoring.
Does not belong here: a second competing template tree or shared platform code.
Type: developer-facing support content.

## What To Copy

Start from `products/savers/_template/`.

That tree now mirrors the real saver product shape:

- `manifest.ini`
- `README.md`
- `presets/`
- `src/`
- `tests/`

It also includes a real entry shim, module hooks, config skeleton, preset/theme descriptors, a simple render/session path, config resources, version resource, and a smoke check.

## Use This Guide With

- [copy-map.md](./copy-map.md) for the rename and replacement checklist
- [validation-checklist.md](./validation-checklist.md) for the minimum static checks before build wiring
- [../manifest-reference.md](../manifest-reference.md) for the canonical file-format rules
- [../saver-authoring.md](../saver-authoring.md) for the full contributor workflow
