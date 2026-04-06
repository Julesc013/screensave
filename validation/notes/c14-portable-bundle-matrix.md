# C14 Portable Bundle Matrix

This note records the final `C14` portable bundle refresh for the current ScreenSave saver line.

## Bundle Architecture

- Source-controlled portable inputs live under `packaging/portable/`.
- Generated staging output lives under `out/portable/screensave-portable-c14-rc/`.
- Generated zip artifact lives under `out/portable/screensave-portable-c14-rc.zip`.
- The portable bundle remains saver-only by policy.
- Under the `C15` doctrine this staged bundle is the pre-freeze Core candidate surface.
- `anthology` is part of the canonical saver line and is staged when its real binary exists.
- `benchlab` and `suite` remain separate app products and are excluded from the end-user saver payload.

## Current Output Discovery

- Canonical saver binaries discovered locally under configured output roots during `C14`: `nocturne`, `ricochet`, `deepfield`, `pipeworks`, `lifeforms`
- No canonical MinGW release or debug saver outputs were discovered during `C14`.
- Legacy pre-rename outputs discovered and excluded: `ember.scr`, `oscilloscope_dreams.scr`
- Local app output discovered and excluded by portable-bundle policy: `benchlab.exe`
- No local `suite.exe` or `anthology.scr` output was discovered during `C14`.

## Portable Inclusion Matrix

| Saver | Expected Artifact | Local Output Status | Staged In C14 Bundle | Manifest Status | Pack Status | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | `nocturne.scr` | found | yes | staged | no file-backed packs | Staged from existing local VS2022 debug output |
| `ricochet` | `ricochet.scr` | found | yes | staged | no file-backed packs | Staged from existing local VS2022 debug output |
| `deepfield` | `deepfield.scr` | found | yes | staged | no file-backed packs | Staged from existing local VS2022 debug output |
| `plasma` | `plasma.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered during `C14` |
| `phosphor` | `phosphor.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `pipeworks` | `pipeworks.scr` | found | yes | staged | no file-backed packs | Staged from existing local VS2022 debug output |
| `lifeforms` | `lifeforms.scr` | found | yes | staged | no file-backed packs | Staged from existing local VS2022 debug output |
| `signals` | `signals.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `mechanize` | `mechanize.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `ecosystems` | `ecosystems.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `stormglass` | `stormglass.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `transit` | `transit.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered during `C14` |
| `observatory` | `observatory.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `vector` | `vector.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `explorer` | `explorer.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `city` | `city.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `atlas` | `atlas.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered during `C14` |
| `gallery` | `gallery.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered during `C14` |
| `anthology` | `anthology.scr` | missing | no | source present only | no file-backed packs | Canonical meta-saver source is present, but no local binary was discovered during `C14` |

## Portable Bundle Notes

- The refreshed bundle is real but partial.
- Missing canonical saver outputs are recorded explicitly in the staged bundle status note instead of being replaced with placeholders.
- The staged zip is real and is generated from the staged folder with built-in tooling.
- `C14` recorded the candidate bundle state before the `C15` doctrine split.
- `C16` will refresh this Core candidate surface and freeze the actual Core baseline.
