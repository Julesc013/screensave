# ScreenSave

ScreenSave is a retro-compatible Windows screensaver platform and product monorepo. Its primary public release is `ScreenSave Core`, a ZIP of standalone `.scr` savers built around a conservative baseline: C89, x86 Win32, the classic ANSI Win32 screensaver host path, mandatory `gdi`, and optional capability-gated OpenGL tiers.

Product doctrine: portable meaning, native delivery, deterministic proof, and optional automation. ScreenSave shares mechanics while preserving product-owned visual meaning; AIDE may coordinate development work, but ScreenSave owns the art, runtime, artifacts, and proof.

The frozen public baseline is `c16-core-baseline`. The current development truth is the Truth And Proof Baseline recorded in [`PROJECT_STATE.toml`](PROJECT_STATE.toml) as `truth-proof-baseline-00`: `ScreenSave Core` remains the released artifact, Plasma's living product posture is the `U09` corrective recut with `GO WITH CAVEATS`, and compatibility claims are now evidence-classed rather than treated as one undifferentiated promise.

Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`.
Post-`SS` work now proceeds in the bounded `SX` substrate series.
`SS00` through `SS02` and `SX00` through `SX08` are complete.
`SS`, `SX`, Plasma `PL`, post-PL `PX`, and the Plasma `U09` corrective recut now exist as lineage and product-truth records. The MX queue files are preserved as historical control-plane records, while the active current-state authority is the truth/proof reset.

## Current Project State

- State authority: [`PROJECT_STATE.toml`](PROJECT_STATE.toml)
- Version authority: [`VERSION.toml`](VERSION.toml)
- Product catalog: [`catalog/products.toml`](catalog/products.toml)
- Artifact profiles: [`catalog/artifact_profiles.toml`](catalog/artifact_profiles.toml)
- Product doctrine: [`contracts/screensave_doctrine_v1.md`](contracts/screensave_doctrine_v1.md)
- AIDE Lite operational profile: [`.aide/pilot.toml`](.aide/pilot.toml), [`.aide/aide_lite.lock.toml`](.aide/aide_lite.lock.toml), and [`.aide/profile.yaml`](.aide/profile.yaml)
- Active milestone: [`docs/roadmap/paw-j-plasma-v2-publication-prep.md`](docs/roadmap/paw-j-plasma-v2-publication-prep.md)
- Accepted portable seam: [`contracts/portable_semantics_v2.md`](contracts/portable_semantics_v2.md)
- Project adapter: [`contracts/project_adapter_v0.md`](contracts/project_adapter_v0.md) and [`tools/project_adapter/screensave_project.py`](tools/project_adapter/screensave_project.py)
- Released public artifact: `c16-core-baseline`
- Development-head product truth: Plasma [`U09 Stable Recut`](products/savers/plasma/docs/u09-stable-recut.md) and [`U09 Ship Posture`](products/savers/plasma/docs/u09-ship-posture.md)
- Plasma ship posture: `GO WITH CAVEATS`
- Queue status: `.codex/queues/wave0.toml` through `.codex/queues/wave5.toml` are historical-superseded lineage records
- Compatibility policy: evidence-classed with `certified`, `binary-audited`, `buildable`, `targeted`, `experimental`, `unsupported`, and `retired` statuses

Use this command for the current machine-readable summary:

```powershell
python tools\scripts\check_project_state.py --summary
```

Use this command surface for external coordinators such as CI or AIDE:

```powershell
python tools\project_adapter\screensave_project.py status
python tools\project_adapter\screensave_project.py catalog
python tools\project_adapter\screensave_project.py validate
python tools\project_adapter\screensave_project.py proof
```

PAW-C Gate C is now accepted for the portable v2 Nocturne/Ricochet canary
slice. Nocturne and Ricochet run through the v2 proof path with unchanged
hashes, current Win32 `.scr` artifacts remain buildable and audited, and AIDE
remains an optional evidence consumer rather than product truth. The active
implementation track is now `plasma-v2-reference-slice`; this opens Plasma v2
planning and implementation without implying release promotion, public SDK
stability, all-saver migration, compatibility certification, or artistic
acceptance.

Plasma v2 is now recorded as `plasma-v2-rc1` in `stable-promoted` state for
the `plasma.v2.reference.preview` profile. The release-candidate package,
proof matrix, stable-promotion package staging, support wording,
provenance/security review, Manager review, Workbench review, instrument audit,
and project-owned final stable artistic acceptance all pass for that scoped
slice. Public release publication, compatibility certification broadening,
public SDK stability, all-saver migration, and platform expansion remain
separate gates.
The next active direction is Plasma v2 publication prep, with the local packet
under [`releases/plasma-v2-stable/`](releases/plasma-v2-stable/). It is not
published and does not broaden compatibility certification. The product remains a
direct-control-first instrument spec island under
`products/savers/plasma/src/v2/`, keeping the old engine as compatibility
evidence rather than the new product center.

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
- `Manager`: optional browser, installer, repair, preview, and saver-settings control product; the historical `suite` source tree is its current implementation home
- `Workbench`: optional authoring, inspection, comparison, profiling, and release-proof product; the current `benchlab` implementation is its Lab Runner seed
- `SDK`: contributor-facing templates, examples, manifests, and authoring guidance
- `Extras`: holdback channel for experimental or lower-confidence material; no frozen payload ships in `C16`

`anthology` is the real suite meta-saver `.scr` and remains part of the standalone saver line rather than the suite app.
`suite` is now the canonical suite-level browser, launcher, preview, and configuration app under `products/apps/suite/`.
`benchlab` and `suite` are now the real non-saver apps in the tree.

## Compatibility Baseline

- Target Windows band: Windows 95, 98, and ME plus Windows NT 4.0 through Windows 11
- Language and binary baseline: C89 and x86 PE32 GUI `.scr` saver artifacts
- Host model: classic ANSI Win32 screensaver command-line and message path
- Guaranteed renderer floor: `gdi`
- Optional accelerated tiers: OpenGL 1.1 (`gl11`) and OpenGL 2.1 (`gl21`), both detected at runtime and subordinate to the universal baseline
- Optional modern tier: OpenGL 3.3 (`gl33`), capability-gated and bounded above the advanced tier without redefining the baseline
- Optional premium tier: OpenGL 4.6 (`gl46`), capability-gated above the modern tier without redefining the baseline
- Internal-only safety tier: `null`

Universal behavior ships first. If an optional capability is absent, ScreenSave must fall back cleanly or keep that behavior out of the universal saver set. Public OS and artifact claims now use evidence classes: `certified`, `binary-audited`, `buildable`, `targeted`, `experimental`, `unsupported`, or `retired`; only `certified` is an ordinary public support promise. For validated coverage, known limits, and the current binary audit, see [`validation/notes/ss02-support-matrix.md`](validation/notes/ss02-support-matrix.md), [`validation/notes/c16-known-issues.md`](validation/notes/c16-known-issues.md), and [`validation/captures/truth-proof-baseline/pe-audit-current.txt`](validation/captures/truth-proof-baseline/pe-audit-current.txt).

## Repository Map

- [`platform/`](platform/): shared runtime, public headers, Win32 host, and renderer implementation
- [`products/`](products/): standalone savers, companion apps, and SDK material
- [`specs/`](specs/): normative repository law and platform contracts
- [`contracts/`](contracts/): versioned portability, renderer-alias, and proof-bundle contracts
- [`docs/`](docs/): architecture notes, release docs, and roadmap material
- [`.github/`](.github/): repository governance, community-health metadata, and stage-appropriate automation
- [`build/`](build/): checked-in toolchain lanes and build coordination docs
- [`packaging/`](packaging/): Core and Installer assembly structure plus release-support material
- [`tests/`](tests/): executable verification logic and fixtures
- [`validation/`](validation/): stored evidence, support notes, and release validation captures
- [`tools/`](tools/): repository validators and support tooling
- [`.aide/`](.aide/): bounded AIDE Lite operational control plane, imported tooling, and ScreenSave-local evidence/work-unit truth
- [`assets/`](assets/): shared non-code content and license records
- [`third_party/`](third_party/): third-party material and license placeholders

## Build And Validation

Checked-in build entry points:

- Default Windows saver solution: [`build/msvc/vs2017_xp/ScreenSave.sln`](build/msvc/vs2017_xp/ScreenSave.sln)
- Companion-tool/dev solution: [`build/msvc/vs2022/ScreenSave.sln`](build/msvc/vs2022/ScreenSave.sln)
- MSBuild example for the default saver lane:

```powershell
py -3 tools\buildctl\screensave_build.py build --profile windows-current-x86
```

- MinGW i686 make lane:

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug
```

Generated output belongs under `out/`, not inside source directories. The checked-in conventions are:

- `out/msvc/vs2017_xp/<Configuration>/<ProjectName>/` for default Windows `.scr` savers
- `out/msvc/vs2022/<Configuration>/<ProjectName>/` for companion tools and modern development builds
- `out/mingw/i686/<profile>/<target>/`
- `out/intermediate/<toolchain>/<lane>/<profile>/<target>/`

Repository-health and release-support validation lives under [`tools/scripts/`](tools/scripts/), while stored release evidence lives under [`validation/`](validation/).

## Further Reading

- Release baseline: [`docs/releases/c16-core-baseline.md`](docs/releases/c16-core-baseline.md)
- Current state authority: [`PROJECT_STATE.toml`](PROJECT_STATE.toml), [`VERSION.toml`](VERSION.toml), and [`docs/roadmap/truth-proof-baseline.md`](docs/roadmap/truth-proof-baseline.md)
- Ultimate project plan: [`docs/roadmap/ultimate-project-report-and-plan.md`](docs/roadmap/ultimate-project-report-and-plan.md)
- Generated catalog inventory: [`catalog/generated/products_inventory.json`](catalog/generated/products_inventory.json), [`catalog/generated/products_sources.mk`](catalog/generated/products_sources.mk), [`catalog/generated/products_table.md`](catalog/generated/products_table.md), and [`catalog/generated/proof_registry.json`](catalog/generated/proof_registry.json)
- Contracts: [`contracts/screensave_doctrine_v1.md`](contracts/screensave_doctrine_v1.md), [`contracts/portable_semantics_v2.md`](contracts/portable_semantics_v2.md), [`contracts/renderer_aliases_v1.md`](contracts/renderer_aliases_v1.md), and [`contracts/proof_bundle_v0.md`](contracts/proof_bundle_v0.md)
- Proof Kernel: [`contracts/proof_kernel_v0.md`](contracts/proof_kernel_v0.md) and [`contracts/surface_rgba8_v0.md`](contracts/surface_rgba8_v0.md)
- Project adapter: [`contracts/project_adapter_v0.md`](contracts/project_adapter_v0.md) and [`tools/project_adapter/README.md`](tools/project_adapter/README.md)
- Release doctrine: [`docs/roadmap/release-channels.md`](docs/roadmap/release-channels.md) and [`docs/roadmap/core-zip-doctrine.md`](docs/roadmap/core-zip-doctrine.md)
- Architecture: [`docs/architecture/overview.md`](docs/architecture/overview.md) and [`docs/architecture/repo-structure.md`](docs/architecture/repo-structure.md)
- Product lineup: [`docs/roadmap/products-and-lineup.md`](docs/roadmap/products-and-lineup.md)
- SDK surface: [`products/sdk/README.md`](products/sdk/README.md)
- Specs: [`specs/README.md`](specs/README.md), [`specs/compatibility.md`](specs/compatibility.md), [`specs/saver_api.md`](specs/saver_api.md), [`specs/renderer_contract.md`](specs/renderer_contract.md), and [`specs/routing_policy.md`](specs/routing_policy.md)
- For contributors: start with [`build/README.md`](build/README.md), [`products/sdk/README.md`](products/sdk/README.md), [`specs/README.md`](specs/README.md), and [`docs/architecture/README.md`](docs/architecture/README.md)

## Project History

The repository has a long implemented history from `S00` through `S15` and a completed `C00` through `C16` continuation bridge, but the root README is no longer the full chronology ledger. Historical milestones such as Series 09, which established the first true multi-product saver family with `ricochet` and `deepfield`, now live in the roadmap docs instead of the front page. For the historical baseline, see [`docs/roadmap/prompt-program.md`](docs/roadmap/prompt-program.md) and [`docs/roadmap/series-map.md`](docs/roadmap/series-map.md). For the current post-`U09` authority, start with [`PROJECT_STATE.toml`](PROJECT_STATE.toml), [`VERSION.toml`](VERSION.toml), [`docs/roadmap/truth-proof-baseline.md`](docs/roadmap/truth-proof-baseline.md), and the Plasma `U09` docs.
