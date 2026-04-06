# C16 Core Inclusion Matrix

This note applies the `C15` Core gate to the current saver line for the frozen `C16` baseline.

## Evidence Basis

- On April 6, 2026, `build/msvc/vs2022/ScreenSave.sln` built successfully for `Release|Win32`.
- The build produced every canonical saver `.scr` target under `out/msvc/vs2022/Release/`.
- The shared host-path expectations still reference `validation/notes/c05-windows-integration-matrix.md`.
- Saver polish, metadata, preset/theme curation, and known product boundaries still reference the completed `C13` and `C14` notes.

## Core Decision Matrix

| Saver | Decision | Fresh C16 Artifact | Gate Summary | Notes |
| --- | --- | --- | --- | --- |
| `nocturne` | included | `out/msvc/vs2022/Release/nocturne/nocturne.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `ricochet` | included | `out/msvc/vs2022/Release/ricochet/ricochet.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `deepfield` | included | `out/msvc/vs2022/Release/deepfield/deepfield.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `plasma` | included | `out/msvc/vs2022/Release/plasma/plasma.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | File-backed packs remain optional bundle content |
| `phosphor` | included | `out/msvc/vs2022/Release/phosphor/phosphor.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `pipeworks` | included | `out/msvc/vs2022/Release/pipeworks/pipeworks.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `lifeforms` | included | `out/msvc/vs2022/Release/lifeforms/lifeforms.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `signals` | included | `out/msvc/vs2022/Release/signals/signals.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `mechanize` | included | `out/msvc/vs2022/Release/mechanize/mechanize.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `ecosystems` | included | `out/msvc/vs2022/Release/ecosystems/ecosystems.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `stormglass` | included | `out/msvc/vs2022/Release/stormglass/stormglass.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `transit` | included | `out/msvc/vs2022/Release/transit/transit.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | File-backed packs remain optional bundle content |
| `observatory` | included | `out/msvc/vs2022/Release/observatory/observatory.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `vector` | included | `out/msvc/vs2022/Release/vector/vector.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `explorer` | included | `out/msvc/vs2022/Release/explorer/explorer.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `city` | included | `out/msvc/vs2022/Release/city/city.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included in frozen Core |
| `atlas` | included | `out/msvc/vs2022/Release/atlas/atlas.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | File-backed packs remain optional bundle content |
| `gallery` | included | `out/msvc/vs2022/Release/gallery/gallery.scr` | Standalone saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Optional `gl21` path remains additive; Core does not depend on it |
| `anthology` | included | `out/msvc/vs2022/Release/anthology/anthology.scr` | Standalone meta-saver target, fresh x86 release build output, `gdi` floor preserved, coherent host/config surface, curated defaults present | Included because it meets the same gate as the other Core savers; it does not replace them |

## Exclusion Summary

- No current saver candidate was deferred to Extras in `C16`.
- Extras remains empty in `C16` because no lower-confidence or experimental saver payload needed to be split out from the current frozen Core lineup.

## Common Limit

- Fresh live Control Panel and per-product runtime smoke were not rerun during `C16`; that limit is recorded in `validation/notes/c16-known-issues.md` rather than being hidden inside the inclusion decision.
