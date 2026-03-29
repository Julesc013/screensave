# C06 Portable Bundle Matrix

This note records the first real portable bundle baseline created in `C06`.

## Bundle Architecture

- Source-controlled portable inputs live under `packaging/portable/`.
- Generated staging output lives under `out/portable/screensave-portable-c06/`.
- Generated zip artifact lives under `out/portable/screensave-portable-c06.zip`.
- BenchLab is excluded from the end-user portable bundle even when `benchlab.exe` exists in local output roots.
- The future `suite` app and suite meta-saver are excluded from this stage.

## Current Output Discovery

- Current canonical saver binaries discovered under `out/msvc/vs2022/Debug/`: `nocturne`, `ricochet`, `deepfield`, `pipeworks`, `lifeforms`
- No canonical MinGW saver outputs were discovered under `out/mingw/i686/debug/`
- Legacy pre-rename outputs discovered and excluded: `ember.scr`, `oscilloscope_dreams.scr`
- BenchLab output discovered and excluded: `out/msvc/vs2022/Debug/benchlab/benchlab.exe`

## Portable Inclusion Matrix

| Saver | Expected Artifact | Current Output Status | Staged In C06 Bundle | Manifest Status | Pack Status | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | `nocturne.scr` | found | yes | staged | no file-backed packs | Included from existing VS2022 debug output; not rebuilt during C06 |
| `ricochet` | `ricochet.scr` | found | yes | staged | no file-backed packs | Included from existing VS2022 debug output; not rebuilt during C06 |
| `deepfield` | `deepfield.scr` | found | yes | staged | no file-backed packs | Included from existing VS2022 debug output; not rebuilt during C06 |
| `plasma` | `plasma.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered |
| `phosphor` | `phosphor.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `pipeworks` | `pipeworks.scr` | found | yes | staged | no file-backed packs | Included from existing VS2022 debug output; not rebuilt during C06 |
| `lifeforms` | `lifeforms.scr` | found | yes | staged | no file-backed packs | Included from existing VS2022 debug output; not rebuilt during C06 |
| `signals` | `signals.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `mechanize` | `mechanize.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `ecosystems` | `ecosystems.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `stormglass` | `stormglass.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `transit` | `transit.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered |
| `observatory` | `observatory.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `vector` | `vector.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `explorer` | `explorer.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `city` | `city.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |
| `atlas` | `atlas.scr` | missing | no | source present only | sample packs source present but not staged | No canonical current output discovered |
| `gallery` | `gallery.scr` | missing | no | source present only | no file-backed packs | No canonical current output discovered |

## Portable Bundle Notes

- The bundle is real but partial.
- Missing canonical saver outputs are recorded explicitly in the staged bundle status note instead of being replaced with placeholders.
- The staged zip is real only because a zip can be produced from the staged folder with built-in tooling, even when the saver payload is partial.
- The next continuation step is `C07` installer, registration, and uninstall flow.
