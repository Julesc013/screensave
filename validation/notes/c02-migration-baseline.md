# C02 Migration Baseline

This note records the locked post-rename baseline created in `C02`.

## Canonical Saver Slugs

- `nocturne`
- `ricochet`
- `deepfield`
- `plasma`
- `phosphor`
- `pipeworks`
- `lifeforms`
- `signals`
- `mechanize`
- `ecosystems`
- `stormglass`
- `transit`
- `observatory`
- `vector`
- `explorer`
- `city`
- `atlas`
- `gallery`

## Canonical App Names

- `benchlab`
- `suite`

`products/apps/player/` remains only as a superseded legacy stub.

## Canonical Renderer Ladder

- `gdi`: guaranteed universal floor
- `gl11`: conservative universal accelerated tier
- `gl21`: first real advanced tier
- `gl33`: later tier placeholder only
- `gl46`: later tier placeholder only
- `null`: internal emergency-only fallback

## Legacy To Canonical Rename Summary

| Legacy | Canonical |
| --- | --- |
| `ember` | `plasma` |
| `oscilloscope_dreams` | `phosphor` |
| `signal_lab` | `signals` |
| `mechanical_dreams` | `mechanize` |
| `night_transit` | `transit` |
| `retro_explorer` | `explorer` |
| `vector_worlds` | `vector` |
| `fractal_atlas` | `atlas` |
| `city_nocturne` | `city` |
| `gl_gallery` | `gallery` |

`Infinity Atlas` is not a separate saver identity. Future atlas-expansion ideas stay under `atlas`.

## Historical References Left Intentionally Historical

- Older changelog entries preserve the saver names and renderer wording that were true when those series landed.
- Older validation notes under `validation/notes/series-*.md` preserve the names and artifact paths that were actually validated at the time.
- The rename table in `docs/roadmap/rename-map.md` intentionally retains both legacy and canonical names as migration guidance.

## Remaining Migration Aliases And Known Risk

- The shared saver registry still translates legacy saver keys to canonical saver keys.
- Renamed savers still read legacy per-product registry roots before saving back to canonical roots.
- `plasma` still accepts the legacy `ember_lava` preset/theme key.
- BenchLab still accepts `gl_plus` and `glplus` as legacy aliases for canonical `gl21`.
- No blocking stale-name residue remains in active repo truth after the `C02` audit.
- Native Windows build and smoke execution were not rerun in this checkpoint environment, so the checkpoint is naming/migration verified rather than newly toolchain-verified.

## Next Continuation Step

- `C03` all-saver `.scr` productization

## Checkpoint

- Annotated git tag for this baseline: `c02-post-rename-baseline`
