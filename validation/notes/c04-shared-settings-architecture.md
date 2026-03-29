# C04 Shared Settings Architecture

This note records the shared settings, presets, themes, randomization, and pack baseline landed in `C04`.

## Shared Taxonomy

| Layer | Shared Contract | Product Ownership |
| --- | --- | --- |
| Basic settings | `schema_version`, `detail_level`, deterministic seed mode/seed, diagnostics overlay, `preset_key`, `theme_key` | Each saver still decides what those choices mean inside its own config |
| Advanced settings | Shared files and dialogs provide a bounded outer contract only | Product-local keys stay inside the owning saver's `[product]` section and config hooks |
| Presets | Versioned `.preset.ini` import/export with shared `[format]`, `[product]`, and `[common]` sections | Product-local behavior entries remain saver-owned |
| Themes | Versioned `.theme.ini` import/export with shared `[format]`, `[product]`, and `[theme]` sections | Theme identity, palette, and style meaning remain saver-owned |
| Randomization | `off` or `session`, with scope flags `preset`, `theme`, `detail`, and `product` | Product-level randomization stays inside saver hooks |
| Packs | Versioned `pack.ini` manifests with relative preset/theme/scene entries | Pack content belongs under the owning saver tree |
| Migration | Shared schema and format versions plus canonical saver-key enforcement on write | Product-local migration stays inside saver hooks and aliases |

## File Formats

- Preset export/import files are human-readable `.preset.ini` files with `kind=preset` and `version=1`.
- Theme export/import files are human-readable `.theme.ini` files with `kind=theme` and `version=1`.
- Pack manifests are human-readable `pack.ini` files with `format=screensave-pack` and `version=1`.
- All shared files carry the canonical saver `product` key plus a `schema_version`.
- Product-local keys remain in `[product]` sections or product-owned scene files.

## Randomization Boundary

- `C04` implements within-saver session randomization only.
- Shared randomization may change preset, theme, detail level, and product-local state when the owning saver opts in.
- Cross-saver randomization remains deferred to later suite meta-saver work.

## Pack Discovery And Failure Rules

- Built-in pack location: `products/savers/<slug>/packs/<pack_key>/pack.ini`
- Portable pack source: reserved in the shared API for later bundle-root discovery
- User pack source: reserved in the shared API for later per-user discovery
- Canonical discovery order: built-in, then portable, then user
- Invalid manifests, unsupported format versions, unsafe entry paths, and broken pack references are ignored with diagnostics instead of crashing or partially loading code

## Host And BenchLab Foundation

- The Win32 fallback settings dialog now exposes shared session-randomization state.
- The Win32 host resolves a per-session effective config so runtime randomization does not overwrite stored settings.
- BenchLab now carries both stored and resolved config and reports active preset, theme, detail, and randomization state in its overlay.
- BenchLab remains a diagnostics harness, not the end-user `suite` app.

## Representative Saver Adoption

Representative deep adoption in `C04` covers:

- `nocturne`
- `plasma`
- `vector`
- `ecosystems`
- `transit`
- `atlas`

These savers now prove:

- product-local preset export/import
- product-local theme coexistence under the shared outer contract
- within-saver randomization hooks
- migration-aware import clamping through the shared saver contract

All current savers advertise the shared settings capability surface so later prompts can extend adoption without reworking the outer contract.

## Deferred After C04

- Windows picker, config, preview, and fullscreen validation on the real `.scr` path
- Portable and user pack root binding in the final distribution layout
- Cross-saver randomization and suite-level orchestration
- Installer, packaging, and any plugin-SDK style extensibility

## Next Continuation Step

- `C05` Windows picker, config, preview, and fullscreen validation
