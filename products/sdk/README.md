# SDK

Purpose: real contributor-facing templates, examples, and authoring guidance for ScreenSave.
Belongs here: saver-authoring docs, pack/preset/theme examples, manifest references, validation guidance, and links to the real saver template.
Does not belong here: private platform internals, generated release artifacts, runtime plugin loaders, or marketplace workflows.
Type: developer-facing support content.

## C10 Baseline

`C10` makes the first real contributor surface live.

Use this area for:

- starting a new first-class saver product with the real template under `products/savers/_template/`
- authoring data-driven preset, theme, and pack content with the shared `C04` formats
- validating contributor-authored saver roots and pack content before build-lane integration
- learning the current role boundaries between standalone savers, `anthology`, `suite`, BenchLab, and internal platform code

This area is intentionally bounded.
Data-driven extensibility is first-class.
Runtime DLL or plugin loading is not the baseline extension model.

## Start Here

- [saver-authoring.md](./saver-authoring.md): how to copy `_template`, rename it safely, and integrate a new saver
- [pack-authoring.md](./pack-authoring.md): how to create preset/theme/pack content without adding runtime code
- [manifest-reference.md](./manifest-reference.md): canonical manifest and file-format guidance
- [contributor-checklist.md](./contributor-checklist.md): compact validation and boundary checklist before opening a change
- [template_saver/README.md](./template_saver/README.md): contributor map for the real saver template
- [examples/README.md](./examples/README.md): minimal example content using the live shared formats

## Validation

Run the contributor helper before wiring a new saver or pack into broader docs or build files:

- `python tools/scripts/check_sdk_surface.py`
- `python tools/scripts/check_sdk_surface.py products/savers/_template`
- `python tools/scripts/check_sdk_surface.py products/sdk/examples/template_pack`

The helper validates the checked-in SDK surface by default and can also validate a contributor-authored saver root, pack directory, `.preset.ini`, or `.theme.ini` path directly.
