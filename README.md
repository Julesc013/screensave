# ScreenSave

ScreenSave is a retro-compatible Windows screensaver platform and product monorepo. Its primary public release is `ScreenSave Core`, a ZIP of standalone `.scr` savers built around a conservative baseline: C89, x86 Win32, the classic ANSI Win32 screensaver host path, mandatory `gdi`, and optional capability-gated OpenGL tiers.

The frozen public baseline is `c16-core-baseline`. `SS00` through `SS02` and `SX00` through `SX01` are complete, active post-`C16` planning now lives under [`docs/roadmap/`](docs/roadmap/), and current release limits remain tracked in [`validation/notes/c16-known-issues.md`](validation/notes/c16-known-issues.md).

Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`.
Post-`SS` work now proceeds in the bounded `SX` substrate series.

## Download ScreenSave Core

- Release page: <https://github.com/Julesc013/screensave/releases/tag/c16-core-baseline>
- Primary download: <https://github.com/Julesc013/screensave/releases/download/c16-core-baseline/screensave-core-c16-baseline.zip>
- Optional installer: <https://github.com/Julesc013/screensave/releases/download/c16-core-baseline/screensave-installer-c16-baseline.zip>
- Checksums and asset manifest: [`releases/c16-core-baseline-sha256.txt`](releases/c16-core-baseline-sha256.txt) and [`releases/c16-core-baseline-manifest.ini`](releases/c16-core-baseline-manifest.ini)
- Release notes and validation summary: [`docs/releases/c16-core-baseline.md`](docs/releases/c16-core-baseline.md), [`validation/notes/ss02-release-validation-summary.md`](validation/notes/ss02-release-validation-summary.md), and [`validation/notes/ss02-support-matrix.md`](validation/notes/ss02-support-matrix.md)

`ScreenSave Core` is the main standalone saver ZIP. `ScreenSave Installer` is a separate current-user convenience package built from the same frozen Core payload.

## What Ships

`ScreenSave Core` includes 19 standalone savers:

- Ambient and motion: `nocturne`, `ricochet`, `deepfield`, `plasma`, `phosphor`
- Grid, systems, and behavior: `pipeworks`, `lifeforms`, `signals`, `mechanize`, `ecosystems`
- Places and atmosphere: `stormglass`, `transit`, `observatory`
- Heavyweight and showcase: `vector`, `explorer`, `city`, `atlas`, `gallery`
- Suite meta-saver: `anthology`

Companion channels stay separate from Core:

- `Installer`: optional installed-distribution path built from the frozen Core payload
- `Suite`: optional browser, launcher, preview, and saver-settings control app
- `BenchLab`: diagnostics and validation harness for renderer and saver inspection
- `SDK`: contributor-facing templates, examples, manifests, and authoring guidance
- `Extras`: holdback channel for experimental or lower-confidence material; no frozen payload ships in `C16`

`anthology` is the real suite meta-saver `.scr` and remains part of the standalone saver line rather than the suite app.
`suite` is now the canonical suite-level browser, launcher, preview, and configuration app under `products/apps/suite/`.
`benchlab` and `suite` are now the real non-saver apps in the tree.

## Compatibility Baseline

- Supported Windows band: Windows 95, 98, and ME plus Windows NT 4.0 through Windows 11
- Language and binary baseline: C89 and x86 PE32 GUI `.scr` saver artifacts
- Host model: classic ANSI Win32 screensaver command-line and message path
- Guaranteed renderer floor: `gdi`
- Optional accelerated tiers: OpenGL 1.1 (`gl11`) and OpenGL 2.1 (`gl21`), both detected at runtime and subordinate to the universal baseline
- Future placeholder tiers: `gl33` and `gl46`
- Internal-only safety tier: `null`

Universal behavior ships first. If an optional capability is absent, ScreenSave must fall back cleanly or keep that behavior out of the universal saver set. For validated coverage and known limits, see [`validation/notes/ss02-support-matrix.md`](validation/notes/ss02-support-matrix.md) and [`validation/notes/c16-known-issues.md`](validation/notes/c16-known-issues.md).

## Repository Map

- [`platform/`](platform/): shared runtime, public headers, Win32 host, and renderer implementation
- [`products/`](products/): standalone savers, companion apps, and SDK material
- [`specs/`](specs/): normative repository law and platform contracts
- [`docs/`](docs/): architecture notes, release docs, and roadmap material
- [`.github/`](.github/): repository governance, community-health metadata, and stage-appropriate automation
- [`build/`](build/): checked-in toolchain lanes and build coordination docs
- [`packaging/`](packaging/): Core and Installer assembly structure plus release-support material
- [`tests/`](tests/): executable verification logic and fixtures
- [`validation/`](validation/): stored evidence, support notes, and release validation captures
- [`tools/`](tools/): repository validators and support tooling
- [`assets/`](assets/): shared non-code content and license records
- [`third_party/`](third_party/): third-party material and license placeholders

## Build And Validation

Checked-in build entry points:

- VS2022 solution: [`build/msvc/vs2022/ScreenSave.sln`](build/msvc/vs2022/ScreenSave.sln)
- MSBuild example:

```powershell
msbuild build\msvc\vs2022\ScreenSave.sln /p:Configuration=Debug /p:Platform=Win32
```

- MinGW i686 make lane:

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug
```

Generated output belongs under `out/`, not inside source directories. The checked-in conventions are:

- `out/msvc/vs2022/<Configuration>/<ProjectName>/`
- `out/mingw/i686/<profile>/<target>/`
- `out/intermediate/<toolchain>/<lane>/<profile>/<target>/`

Repository-health and release-support validation lives under [`tools/scripts/`](tools/scripts/), while stored release evidence lives under [`validation/`](validation/).

## Further Reading

- Release baseline: [`docs/releases/c16-core-baseline.md`](docs/releases/c16-core-baseline.md)
- Release doctrine: [`docs/roadmap/release-channels.md`](docs/roadmap/release-channels.md) and [`docs/roadmap/core-zip-doctrine.md`](docs/roadmap/core-zip-doctrine.md)
- Architecture: [`docs/architecture/overview.md`](docs/architecture/overview.md) and [`docs/architecture/repo-structure.md`](docs/architecture/repo-structure.md)
- Product lineup: [`docs/roadmap/products-and-lineup.md`](docs/roadmap/products-and-lineup.md)
- SDK surface: [`products/sdk/README.md`](products/sdk/README.md)
- Specs: [`specs/README.md`](specs/README.md), [`specs/compatibility.md`](specs/compatibility.md), [`specs/saver_api.md`](specs/saver_api.md), and [`specs/renderer_contract.md`](specs/renderer_contract.md)
- For contributors: start with [`build/README.md`](build/README.md), [`products/sdk/README.md`](products/sdk/README.md), [`specs/README.md`](specs/README.md), and [`docs/architecture/README.md`](docs/architecture/README.md)

## Project History

The repository has a long implemented history from `S00` through `S15` and a completed `C00` through `C16` continuation bridge, but the root README is no longer the full chronology ledger. Historical milestones such as Series 09, which established the first true multi-product saver family with `ricochet` and `deepfield`, now live in the roadmap docs instead of the front page. For the historical baseline, see [`docs/roadmap/prompt-program.md`](docs/roadmap/prompt-program.md) and [`docs/roadmap/series-map.md`](docs/roadmap/series-map.md). For the active post-`C16` handoff, see [`docs/roadmap/post-c16-program.md`](docs/roadmap/post-c16-program.md) and [`docs/roadmap/sx-series.md`](docs/roadmap/sx-series.md).
