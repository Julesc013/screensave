# C10 SDK Contributor Surface

This note records the first real contributor-facing SDK surface landed in `C10`.

## Included Surface

- `products/savers/_template/` is now the canonical starter saver tree.
- `products/sdk/` now contains saver-authoring guidance, pack-authoring guidance, manifest references, and contributor checklists.
- `products/sdk/examples/template_pack/` is the minimal example pack for the shared preset/theme/pack format.
- `tools/scripts/check_sdk_surface.py` validates the checked-in contributor surface and contributor-authored saver or pack roots.

## Authoring Boundaries

| Area | Supported In `C10` | Explicit Non-Goal |
| --- | --- | --- |
| New saver products | Copy and rename `products/savers/_template/` into a first-class in-repo saver | Runtime plugin or DLL loading |
| Presets and themes | Versioned `.preset.ini` and `.theme.ini` files | Executable extension code |
| Packs | Human-readable `pack.ini` manifests with relative preset and theme entries | Marketplace or remote distribution |
| Diagnostics | BenchLab and the validator scripts help inspect authored products | BenchLab as an authoring IDE |
| Suite app | Browse, launch, preview, and configuration handoff for existing products | Saver or pack authoring workflow |

## Validation Expectations

- Run `python tools/scripts/check_sdk_surface.py` before treating the checked-in SDK surface as healthy.
- Run `python tools/scripts/check_sdk_surface.py <path-to-new-saver-root>` before adding a contributor-authored saver to build files or truth-bearing docs.
- Run `python tools/scripts/check_sdk_surface.py <path-to-pack-root>` before claiming a pack is ready for portable or installed discovery.
- Keep renderer claims honest: `gdi` mandatory, `gl11` optional, `gl21` only when real, `gl33` and `gl46` future-only.

## Current Limits

- `C10` does not add a runtime plugin loader or compiled extension API.
- `C10` does not auto-generate build targets for new savers.
- `C10` validation is static-only in this environment; no fresh MSVC or MinGW toolchain build was available here.

## Next Continuation Step

- `C11` backlog ingestion and routing of new ideas
