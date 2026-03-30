# C14 Release-Readiness Matrix

This note records the final `C14` release-readiness picture for the current ScreenSave suite.

## Saver Line

| Product | Kind | Checked-In Target | Local Output Discovered In C14 | Portable / Installer Presence | Evidence Level | RC Status | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | saver | yes | yes | yes / yes | static plus local binary discovery | partial binary-ready | Included in the refreshed staged bundle and installer payload |
| `ricochet` | saver | yes | yes | yes / yes | static plus local binary discovery | partial binary-ready | Included in the refreshed staged bundle and installer payload |
| `deepfield` | saver | yes | yes | yes / yes | static plus local binary discovery | partial binary-ready | Included in the refreshed staged bundle and installer payload |
| `plasma` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `phosphor` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `pipeworks` | saver | yes | yes | yes / yes | static plus local binary discovery | partial binary-ready | Included in the refreshed staged bundle and installer payload |
| `lifeforms` | saver | yes | yes | yes / yes | static plus local binary discovery | partial binary-ready | Included in the refreshed staged bundle and installer payload |
| `signals` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `mechanize` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `ecosystems` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `stormglass` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `transit` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `observatory` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `vector` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `explorer` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `city` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `atlas` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `gallery` | saver | yes | no | no / no | static only | source-complete, output-unavailable here | Product source and validation notes are present, but no local binary was discovered |
| `anthology` | meta-saver | yes | no | no / no | static only | source-complete, output-unavailable here | Canonical meta-saver source is present, but no local binary was discovered |

## App And SDK Surface

| Product | Kind | Checked-In Target | Local Output Discovered In C14 | Distribution Presence | Evidence Level | RC Status | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `benchlab` | app | yes | yes | excluded from end-user packages | static plus local binary discovery | developer-only local binary | Real diagnostics harness output was discovered locally and remains outside end-user packages |
| `suite` | app | yes | no | excluded from end-user packages | static only | source-complete, output-unavailable here | Real suite app source and build target exist, but no local `suite.exe` was discovered |
| `sdk` | sdk | yes | n/a | source-only | docs plus validator evidence | source-ready | Contributor surface is source-controlled and validated separately from runtime outputs |

## Renderer And Fallback Readiness

| Area | Current State | Evidence Level | Note |
| --- | --- | --- | --- |
| `gdi` | required floor | historical runtime plus current static validation | Remains the guaranteed baseline |
| `gl11` | optional | historical runtime plus current static validation | Capability-gated and subordinate to `gdi` |
| `gl21` | optional | static only in this environment | Capability-gated advanced tier; no fresh local runtime evidence in `C14` |
| `gl33` / `gl46` | placeholders | static contract only | Not real backends and not release requirements |
| Fallback wording | normalized | static validation | Shared renderer and diagnostics vocabulary stayed aligned through `C14` |

## Release Summary

- The source tree is converged and coherently documented through `C14`.
- The staged portable and installer packages are real but partial because the current local output roots contain only five canonical saver binaries.
- BenchLab is available as a local developer artifact in this environment; Suite is source-complete but has no discovered local binary here.
- Known limitations are recorded explicitly in `validation/notes/c14-known-issues.md`.
