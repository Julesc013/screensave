# C03 Productization Matrix

This note records the locked saver-product matrix created in `C03`.

## Saver Product Line

| Saver | VS2022 Target | MinGW Target | Output | Entry Binding | Resources | Config Routing |
| --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | `nocturne.vcxproj` | `nocturne` | `nocturne.scr` | `nocturne_entry.c -> screensave_scr_main(nocturne_get_module())` | `nocturne_config.rc`, `nocturne_version.rc`, `screensave_host.rc` | Single-saver host path through `nocturne_get_module()` |
| `ricochet` | `ricochet.vcxproj` | `ricochet` | `ricochet.scr` | `ricochet_entry.c -> screensave_scr_main(ricochet_get_module())` | `ricochet_config.rc`, `ricochet_version.rc`, `screensave_host.rc` | Single-saver host path through `ricochet_get_module()` |
| `deepfield` | `deepfield.vcxproj` | `deepfield` | `deepfield.scr` | `deepfield_entry.c -> screensave_scr_main(deepfield_get_module())` | `deepfield_config.rc`, `deepfield_version.rc`, `screensave_host.rc` | Single-saver host path through `deepfield_get_module()` |
| `plasma` | `plasma.vcxproj` | `plasma` | `plasma.scr` | `plasma_entry.c -> screensave_scr_main(plasma_get_module())` | `plasma_config.rc`, `plasma_version.rc`, `screensave_host.rc` | Single-saver host path through `plasma_get_module()` |
| `phosphor` | `phosphor.vcxproj` | `phosphor` | `phosphor.scr` | `phosphor_entry.c -> screensave_scr_main(phosphor_get_module())` | `phosphor_config.rc`, `phosphor_version.rc`, `screensave_host.rc` | Single-saver host path through `phosphor_get_module()` |
| `pipeworks` | `pipeworks.vcxproj` | `pipeworks` | `pipeworks.scr` | `pipeworks_entry.c -> screensave_scr_main(pipeworks_get_module())` | `pipeworks_config.rc`, `pipeworks_version.rc`, `screensave_host.rc` | Single-saver host path through `pipeworks_get_module()` |
| `lifeforms` | `lifeforms.vcxproj` | `lifeforms` | `lifeforms.scr` | `lifeforms_entry.c -> screensave_scr_main(lifeforms_get_module())` | `lifeforms_config.rc`, `lifeforms_version.rc`, `screensave_host.rc` | Single-saver host path through `lifeforms_get_module()` |
| `signals` | `signals.vcxproj` | `signals` | `signals.scr` | `signals_entry.c -> screensave_scr_main(signals_get_module())` | `signals_config.rc`, `signals_version.rc`, `screensave_host.rc` | Single-saver host path through `signals_get_module()` |
| `mechanize` | `mechanize.vcxproj` | `mechanize` | `mechanize.scr` | `mechanize_entry.c -> screensave_scr_main(mechanize_get_module())` | `mechanize_config.rc`, `mechanize_version.rc`, `screensave_host.rc` | Single-saver host path through `mechanize_get_module()` |
| `ecosystems` | `ecosystems.vcxproj` | `ecosystems` | `ecosystems.scr` | `ecosystems_entry.c -> screensave_scr_main(ecosystems_get_module())` | `ecosystems_config.rc`, `ecosystems_version.rc`, `screensave_host.rc` | Single-saver host path through `ecosystems_get_module()` |
| `stormglass` | `stormglass.vcxproj` | `stormglass` | `stormglass.scr` | `stormglass_entry.c -> screensave_scr_main(stormglass_get_module())` | `stormglass_config.rc`, `stormglass_version.rc`, `screensave_host.rc` | Single-saver host path through `stormglass_get_module()` |
| `transit` | `transit.vcxproj` | `transit` | `transit.scr` | `transit_entry.c -> screensave_scr_main(transit_get_module())` | `transit_config.rc`, `transit_version.rc`, `screensave_host.rc` | Single-saver host path through `transit_get_module()` |
| `observatory` | `observatory.vcxproj` | `observatory` | `observatory.scr` | `observatory_entry.c -> screensave_scr_main(observatory_get_module())` | `observatory_config.rc`, `observatory_version.rc`, `screensave_host.rc` | Single-saver host path through `observatory_get_module()` |
| `vector` | `vector.vcxproj` | `vector` | `vector.scr` | `vector_entry.c -> screensave_scr_main(vector_get_module())` | `vector_config.rc`, `vector_version.rc`, `screensave_host.rc` | Single-saver host path through `vector_get_module()` |
| `explorer` | `explorer.vcxproj` | `explorer` | `explorer.scr` | `explorer_entry.c -> screensave_scr_main(explorer_get_module())` | `explorer_config.rc`, `explorer_version.rc`, `screensave_host.rc` | Single-saver host path through `explorer_get_module()` |
| `city` | `city.vcxproj` | `city` | `city.scr` | `city_entry.c -> screensave_scr_main(city_get_module())` | `city_config.rc`, `city_version.rc`, `screensave_host.rc` | Single-saver host path through `city_get_module()` |
| `atlas` | `atlas.vcxproj` | `atlas` | `atlas.scr` | `atlas_entry.c -> screensave_scr_main(atlas_get_module())` | `atlas_config.rc`, `atlas_version.rc`, `screensave_host.rc` | Single-saver host path through `atlas_get_module()` |
| `gallery` | `gallery.vcxproj` | `gallery` | `gallery.scr` | `gallery_entry.c -> screensave_scr_main(gallery_get_module())` | `gallery_config.rc`, `gallery_version.rc`, `screensave_host.rc` | Single-saver host path through `gallery_get_module()` |

## Shared Productization Rules

- Each saver target now compiles only the owning saver sources, not the full saver lineup.
- Each saver target keeps the shared platform library, shared host resource, and product-local config/version resources.
- `screensave_scr_main_with_registry()` remains in the host only for explicit multi-saver contexts; the distributed saver line uses `screensave_scr_main()` with a single module.
- BenchLab remains a separate diagnostics app and continues to link saver modules directly instead of launching distributed `.scr` binaries.

## Deferred After C03

- Shared settings, presets, randomization, and pack architecture
- Windows picker, config, preview, and fullscreen validation
- Packaging, installer, and registration work
- `suite` app and suite-level meta saver work

## Next Continuation Step

- `C04` shared settings, presets, randomization, and pack architecture
