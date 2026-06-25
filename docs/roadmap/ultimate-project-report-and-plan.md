# ScreenSave Ultimate Project Report And Plan

Status: active synthesis and planning report.

Date: 2026-06-25.

Primary doctrine: [../../contracts/screensave_doctrine_v1.md](../../contracts/screensave_doctrine_v1.md).

Machine-readable state: [../../PROJECT_STATE.toml](../../PROJECT_STATE.toml).

## Executive Summary

ScreenSave is no longer a small single-saver experiment. It is a product
monorepo for a retro-compatible family of native screensavers and companion
tools.

The best synthesis is:

```text
Portable meaning. Native delivery. Deterministic proof. Optional automation.
```

The product-led formulation is:

```text
ScreenSave is a curated family of autonomous native generative artworks,
built from portable deterministic semantics, delivered through thin native
hosts, extended primarily through validated data, and qualified by one shared
proof system.
```

The crucial boundary is:

```text
ScreenSave owns the art, runtime, artifacts, compatibility, and proof.
AIDE may coordinate bounded development activity around ScreenSave.
```

The repository has already implemented a substantial baseline:

- a C89, x86 Win32 `.scr`, ANSI-host compatibility doctrine
- 19 standalone saver products
- `anthology` as a standalone meta-saver
- optional companion surfaces for installer, suite/Manager, BenchLab/Workbench,
  and SDK
- the public renderer ladder `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, and
  internal `null`
- Core release doctrine and frozen `c16-core-baseline`
- completed `SS`, `SX`, `PL`, `PX`, and Plasma `U09` lineage
- evidence-classed compatibility language
- project-state and version authority
- product catalog and artifact profiles
- generated catalog inventory
- Proof Kernel v0 Nocturne canary
- compiled Nocturne canary runner
- `sslab` render and comparison tooling
- ScreenSave project adapter commands for external coordinators
- hardened fixed-capability adapter output containment
- Product Architecture v1 contract for four planes and canary sequence
- VisualIntent v1 draft authoring contract
- pinned report-only AIDE Lite lock
- report-only AIDE pilot integration

The repository is not complete. The largest remaining work is not more
architecture prose. It is a sequence of proof-backed product slices:

- finish Proof Kernel v0 as a complete Nocturne vertical slice
- add Ricochet as the second architectural canary
- move to a host-neutral portable v2 seam only after multiple products prove it
- implement the VisualIntent resolver and safe pack generation after proof exists
- build Workbench over the same runner as CI
- evolve Manager from the current suite lineage
- certify compatibility from evidence instead of target language
- expand native platforms only through semantic cores plus native hosts

## Current Truth

Current authority is split deliberately:

| Surface | Current Authority |
| --- | --- |
| Project state | [../../PROJECT_STATE.toml](../../PROJECT_STATE.toml) |
| Version authority | [../../VERSION.toml](../../VERSION.toml) |
| Product doctrine | [../../contracts/screensave_doctrine_v1.md](../../contracts/screensave_doctrine_v1.md) |
| Product architecture | [../../contracts/product_architecture_v1.md](../../contracts/product_architecture_v1.md) |
| Product catalog | [../../catalog/products.toml](../../catalog/products.toml) |
| Generated catalog inventory | [../../catalog/generated/products_inventory.json](../../catalog/generated/products_inventory.json) |
| Artifact profiles | [../../catalog/artifact_profiles.toml](../../catalog/artifact_profiles.toml) |
| Proof Kernel v0 | [../../contracts/proof_kernel_v0.md](../../contracts/proof_kernel_v0.md) |
| RGBA8 surface contract | [../../contracts/surface_rgba8_v0.md](../../contracts/surface_rgba8_v0.md) |
| Project adapter | [../../contracts/project_adapter_v0.md](../../contracts/project_adapter_v0.md) |
| VisualIntent draft | [../../contracts/visual_intent_v1.md](../../contracts/visual_intent_v1.md) |
| Report-only AIDE pilot | [../../.aide/pilot.toml](../../.aide/pilot.toml) |
| Pinned AIDE Lite lock | [../../.aide/aide_lite.lock.toml](../../.aide/aide_lite.lock.toml) |
| Plasma stable truth | [../../products/savers/plasma/docs/u09-stable-recut.md](../../products/savers/plasma/docs/u09-stable-recut.md) |
| Plasma ship posture | [../../products/savers/plasma/docs/u09-ship-posture.md](../../products/savers/plasma/docs/u09-ship-posture.md) |

Current state summary:

- released public artifact: `c16-core-baseline`
- development head: `plasma-u09-corrective-recut`
- active program: `proof-kernel-v0`
- release candidate: none
- queue authority: historical-superseded MX queues
- compatibility policy: evidence-classed
- Plasma posture: `GO_WITH_CAVEATS`

The broad Windows band remains a target, not a certified claim.
Only `certified` evidence should read as ordinary public compatibility support.

## Product Doctrine

The doctrine is now frozen enough to stop relitigating:

- Core is the primary product.
- Standalone native savers remain autonomous.
- Anthology remains a saver.
- Manager and Workbench remain optional.
- Product semantics remain product-owned.
- Software/reference rendering is mandatory.
- Acceleration is optional and capability-gated.
- Data packages are the default extension mechanism.
- Arbitrary runtime plugins are not a baseline feature.
- Compatibility is artifact-specific and evidence-backed.
- Workbench must use the same runner as CI.
- AIDE is external development infrastructure, not product runtime.

The product/platform rule is:

```text
Share mechanics.
Preserve meaning.
```

The platform may share time, deterministic random-number generation,
configuration primitives, capability reporting, diagnostics, surfaces,
presentation contracts, native host lifecycle, content loading, and proof
hooks.

Each saver product owns its visual thesis, state model, simulation,
configuration semantics, deterministic runtime plan, reference rendering rules,
degradation policy, and visual acceptance criteria.

## Completed History

The completed baseline line is:

| Series | Result |
| --- | --- |
| `S00` | Constitution, specs, docs, AGENTS hierarchy, and permanent skeleton. |
| `S01` | Governance, contribution path, workflows, and repository validation. |
| `S02` | VS2022 and MinGW build lanes plus output conventions. |
| `S03` | Classic Win32 `.scr` host skeleton and lifecycle routing. |
| `S04` | Shared core runtime and first public platform API layer. |
| `S05` | Mandatory GDI backend and shared present path. |
| `S06` | `nocturne` as the first real saver. |
| `S07` | BenchLab and deterministic diagnostics instrumentation. |
| `S08` | Optional GL11 backend and fallback reporting. |
| `S09` | Motion family with `ricochet` and `deepfield`. |
| `S10` | Framebuffer/vector family with `plasma` and `phosphor`. |
| `S11` | Grid/simulation family with `pipeworks` and `lifeforms`. |
| `S12` | Systems/ambient family with `signals`, `mechanize`, and `ecosystems`. |
| `S13` | Places/atmosphere family. |
| `S14` | Heavyweight worlds family. |
| `S15` | GL21, GL33/GL46 ladder entries, internal null, and `gallery`. |

The completed continuation bridge is:

| Series | Result |
| --- | --- |
| `C00` | Roadmap reset and normalization planning. |
| `C01` | Canonical saver/product/app naming and renderer terminology. |
| `C02` | Post-rename checkpoint baseline. |
| `C03` | All-saver `.scr` productization. |
| `C04` | Shared settings, presets, randomization, and pack architecture. |
| `C05` | Windows picker, config, preview, and fullscreen validation. |
| `C06` | Portable distribution bundle. |
| `C07` | Installer, registration, and uninstall flow. |
| `C08` | Suite-level meta-saver. |
| `C09` | `suite` app. |
| `C10` | SDK and contributor surface. |
| `C11` | Idea backlog ingestion. |
| `C12` | Cross-cutting polish. |
| `C13` | Per-family polish waves. |
| `C14` | Rerelease hardening. |
| `C15` | Release doctrine and channel split. |
| `C16` | Core release refresh and baseline freeze. |

The post-C16 lineage then continued through:

- `SS00` through `SS02`: closed release-closure bridge
- `SX00` through `SX09`: closed substrate-hardening and capability-expansion
  series
- `PL00` through `PL14`: closed Plasma flagship expansion line
- `PX` and `U01` through `U09`: Plasma corrective, curation, and ship-posture
  line
- Truth and Proof Baseline: current authority convergence milestone
- Proof Kernel v0: current execution and proof milestone

The most recent implemented authority/proof work added:

- `.gitattributes`
- `PROJECT_STATE.toml`
- `VERSION.toml`
- product catalog and artifact profiles
- generated catalog inventory
- PE audit tooling
- report-only AIDE pilot
- pinned AIDE Lite lock
- portable seam and proof-bundle draft contracts
- Proof Kernel v0 Nocturne canary
- private RGBA8 and soft-render primitives
- compiled Nocturne canary runner
- `sslab` render and compare
- ScreenSave Doctrine v1
- Product Architecture v1
- VisualIntent v1 draft authoring contract
- ScreenSave project adapter with `status`, `capabilities`, `catalog`,
  `validate`, `render`, `compare`, `audit`, and `proof`
- fixed-capability adapter containment for generated outputs, compare inputs,
  and artifact-profile PE audit roots

## Current Product Inventory

The generated catalog reports 22 products:

| Category | Products |
| --- | --- |
| Core savers | `nocturne`, `ricochet`, `deepfield`, `plasma`, `phosphor`, `pipeworks`, `lifeforms`, `signals`, `mechanize`, `ecosystems`, `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, `anthology` |
| Companion apps | `suite`, `benchlab` |
| SDK | `sdk` |

The current artifact profiles are:

| Profile | Role |
| --- | --- |
| `windows_preservation_x86_ansi` | targeted preservation `.scr` profile |
| `windows_current_x86_scr` | targeted current Windows `.scr` profile |
| `windows_current_tool` | current companion app profile |
| `source_sdk` | contributor-facing SDK source profile |

## Current Implementation Surfaces

### Runtime And Product

Implemented:

- product-local saver trees under `products/savers/`
- public platform headers and Win32 host/platform shape
- GDI floor
- optional GL ladder
- product manifests, presets, themes, packs, and content scaffolding
- anthology meta-saver lineage
- suite app lineage
- benchlab diagnostics lineage
- SDK templates and contributor docs

Caveats:

- public saver APIs still expose Win32 concepts in places
- portability boundary is not yet v2-clean
- software/reference proof path is only canary-grade
- broad visual regression proof is not yet general
- Manager and Workbench are not yet final product names/UX implementations

### Proof Kernel v0

Implemented:

- private RGBA8 surface contract
- private soft-render primitive contract
- Nocturne `observatory_night` proof canary
- deterministic capture hash:
  `5394a14b6622c17bfb10cd5721c08a4c92cdbddfb12f55c954ef1d5f6ef878b2`
- `sslab render`
- `sslab compare`
- proof-bundle v0 JSON for the canary
- compiled Nocturne canary runner over the private C proof primitives
- adapter proof receipts that include artifact-profile PE audit facts
- validator for deterministic repeatability and exact comparison

Caveats:

- `sslab.py` remains the current proof CLI; the compiled canary validates the
  private C primitives but is not yet a reusable `libsslab`
- Nocturne is the only proof-kernel product canary
- tolerant/perceptual comparison classes exist as report helpers, not accepted
  validation gates
- lifecycle, resize, soak, resource, and performance proof are not yet in
  Proof Kernel v0

### Project Adapter

Implemented commands:

```text
status
capabilities
catalog
validate
render
compare
audit
proof
```

The adapter delegates to ScreenSave-owned tools. It does not edit code, merge
branches, publish releases, certify compatibility, or accept visual quality.

The adapter is the correct integration surface for AIDE, CI, and future worker
harnesses.

### AIDE Pilot

Implemented:

- report-only `.aide/` pilot
- pinned `.aide/aide_lite.lock.toml`
- project boundary that forbids saver runtime dependency
- WorkUnit/evidence framing direction
- project adapter that AIDE can call without owning ScreenSave truth
- fixed-capability bridge profile and evidence packet for deterministic
  command admission

Not implemented:

- autonomous ScreenSave development loop
- automatic merge
- AIDE-owned test broker for ScreenSave
- AIDE-defined visual semantics
- AIDE-dependent product build or release

## Ultimate Product Family

The intended end-state product family is:

| Product | Purpose | Completion Standard |
| --- | --- | --- |
| Core | Curated standalone native savers | Certified artifacts, proof bundles, installer-independent ZIP release |
| Anthology | Standalone deterministic meta-saver | Cross-saver sequencing proof and stable configuration |
| Manager | Browse, install, repair, preview, configure, and manage packs | Optional user product over catalog/install/config surfaces |
| Workbench | Author, inspect, capture, compare, profile, and qualify | GUI over `libsslab`/proof kernel, not a duplicate validation engine |
| SDK | Contracts, schemas, examples, generators, and validation guidance | Contributor path that can produce valid savers and packs |
| Labs | Experimental savers, renderers, modules, and content | Explicit holdback channel, never silently promoted |
| Preservation | Historical artifacts, toolchains, VM profiles, and compatibility evidence | Separate support channel with evidence classes |

Core remains primary. Everything else is optional.

## Ultimate Architecture

ScreenSave has four planes:

```text
Product plane
  Core / Anthology / Manager / Workbench / SDK / Labs / Preservation

Semantic and runtime plane
  VisualIntent
    -> product-family resolver
    -> product-specific specification
    -> deterministic runtime plan
    -> reference or optional accelerated compute
    -> rendering realization
    -> explicit surface
    -> presenter
    -> native host or headless host

Proof plane
  catalog
    + build profiles
    + artifact audit
    + fixed-step runner
    + RGBA reference surface
    + deterministic capture
    + image comparison
    + lifecycle/performance/soak evidence
    + compatibility evidence
    -> proof bundle

Development plane
  Git / CI / project adapter / ScreenSave proof bundles
    -> optional AIDE WorkUnits and EvidencePackets
    -> optional worker harnesses
```

The semantic rule is:

```text
One stable platform contract.
Many product-owned semantic cores.
Multiple shells.
```

There must not be one universal graphics graph, one mandatory Workbench, one
central product database, one embedded AIDE runtime, or one lowest-common-
denominator cross-platform shell.

## Ultimate Command Surface

The current adapter is a partial implementation of the desired stable command
surface.

| Command | Current State | Target |
| --- | --- | --- |
| `status` | implemented by project adapter | stable |
| `catalog` | implemented by project adapter | stable and used by Manager/Workbench |
| `validate` | implemented by project adapter | broader validation ladder |
| `render` | implemented for Nocturne canary | product-generic headless rendering |
| `compare` | implemented for PPM captures | exact, tolerant, perceptual, observational proof classes |
| `audit` | implemented for PE facts | artifact-profile aware audit gates |
| `proof` | implemented for Nocturne exact canary | full proof-bundle exporter |
| `build` | not yet implemented | profile-aware build command |
| `lifecycle` | not yet implemented | create/destroy/resize/suspend/resume proof |
| `profile` | not yet implemented | frame-time/resource/handle metrics |
| `package` | not yet implemented | Core, Manager, Workbench, SDK, Preservation packaging |

The current project adapter now exposes fixed capability bindings in
`tools/project_adapter/capability_bindings.json`, command-specific receipt
requirements in `tools/project_adapter/receipt_schemas.json`, and contained
generated-output roots under `out/aide/screensave-project-adapter/invocations/`.
PE audit roots are selected through named artifact profiles, not arbitrary
caller-supplied paths. This is
the ScreenSave-owned bridge profile AIDE can consume without being allowed to
run arbitrary ScreenSave commands or choose arbitrary output paths.

## Major Gaps

### Product And Runtime Gaps

- no portable v2 host-neutral saver contract is implemented yet
- public headers still expose Win32-specific types
- no v1 compatibility adapter wraps a new v2 semantic contract
- product-local IRs are not formalized across canaries
- Nocturne now has a compiled proof-kernel canary, but not yet a reusable
  compiled `libsslab` runner path
- Ricochet and Plasma are not yet proof-kernel canaries
- native macOS/Linux hosts do not exist
- WebAssembly preview does not exist

### Proof Gaps

- no general headless runner library
- no lifecycle test command
- no performance percentile metrics
- no resource/handle monitoring
- no soak runner
- no artifact-profile-aware PE gate beyond fact reporting
- no PNG/raw-frame formal capture profile beyond current PPM canary
- no visual proof bundles for all savers
- no cross-renderer equivalence policy
- no CI integration for proof bundles

### Manager And Workbench Gaps

- current `suite` lineage is not yet a final Manager product
- current `benchlab` lineage is not yet a final Workbench product
- Workbench does not yet consume a stable `libsslab`
- Manager does not yet consume compiled packs, safe caches, rollback, or
  provenance surfaces as final UX
- no unified author/inspect/compare/profile/release Workbench UX exists

### Content And Modding Gaps

- VisualIntent v1 is documented as a draft authoring contract, but no resolver
  or compiler consumes it yet
- product-specific authoring schemas are not formalized as stable pack inputs
- pack compiler is not implemented as a definitive safe data pipeline
- text/image/video-to-intent flows are not implemented
- generated candidates are not proof-qualified
- runtime executable plugins remain deferred, correctly

### AIDE And Automation Gaps

- AIDE remains report-only
- `.aide/aide_lite.lock.toml` pins the admitted AIDE Lite identity, but AIDE
  remains optional and removable
- the fixed-command bridge profile exists, but no external AIDE-side runtime
  admission is complete in this repository
- no ScreenSave-specific AIDE EvidencePacket export from live adapter receipts
  is complete
- no autonomous canary should run until ScreenSave proof bundles are stronger
- no automatic merge should be allowed until deterministic acceptance classes
  exist for the task class

### Compatibility Gaps

- current broad Windows band is targeted, not certified
- preservation x86 ANSI requires real legacy VM or hardware evidence
- current binaries need artifact-profile-specific audit and runtime proof
- old toolchains, VM profiles, and preservation evidence need a separate
  maintained lane
- macOS, Linux, ARM64, and Web targets are future targets only

## Completion Definition

ScreenSave is "all complete" only when all of these are true:

- Core ships as curated standalone native saver artifacts.
- Every Core saver has a deterministic reference path.
- Every Core saver has proof bundles for declared support profiles.
- Compatibility claims are certified, binary-audited, buildable, targeted,
  experimental, unsupported, or retired with stored evidence.
- Anthology is proved as an autonomous saver.
- Manager is optional and can install, repair, preview, configure, and manage
  packs without becoming mandatory.
- Workbench is optional and uses the same proof runner as CI.
- SDK can guide contributors to produce valid savers and packs.
- Labs is clearly separated from stable Core.
- Preservation is separated from current-platform support.
- AIDE can coordinate ScreenSave work through project-owned commands.
- Deleting `.aide/` does not prevent ScreenSave from building, testing, or
  releasing.

## Roadmap To Complete

### Gate 0 - Authority And Truth

Status: mostly complete, ongoing guard.

Done:

- `PROJECT_STATE.toml`
- `VERSION.toml`
- `.gitattributes`
- catalog and artifact profiles
- generated catalog inventory
- evidence-classed compatibility doctrine
- report-only AIDE pilot
- project adapter

Remaining:

- keep generated catalog inventory current
- keep release/development distinction clear
- keep PE audit and support matrix current
- avoid new roadmap families without closing current gates

### Gate 1 - Proof Kernel v0

Goal: complete Nocturne proof-kernel vertical slice.

Done:

- Nocturne canary
- private RGBA8 contract
- private soft renderer primitives
- `sslab render`
- `sslab compare`
- deterministic PPM capture and hash
- compiled C canary over private proof primitives
- artifact-profile PE audit facts in adapter proof receipts
- adapter `render`, `compare`, and `proof`

Remaining:

- extract a reusable compiled `libsslab` path from the canary-grade primitives
- decide final v0 capture format policy: PPM, raw RGBA, PNG, or multiple
- add basic lifecycle smoke for Nocturne without widening v0 too far
- update proof-bundle schema from draft to implemented v0 if needed
- make CI run the v0 proof command

Exit:

- `python tools\project_adapter\screensave_project.py proof` emits a complete
  proof bundle from ScreenSave-owned reference infrastructure and records
  artifact-profile binary facts
- repeated runs compare exactly
- no public compatibility claim is widened

### Gate 2 - Proof Kernel v1 And Ricochet Canary

Goal: prove the architecture is not Nocturne-only.

Required:

- Ricochet or equivalent stateful non-field saver headless canary
- resize behavior
- create/destroy lifecycle cycling
- fixed-step replay
- frame-time statistics
- resource and handle metrics
- tolerant/perceptual comparison admission rules
- CI integration

Exit:

- Nocturne and Ricochet both run through the same proof library
- shared abstractions are justified by two structurally different products

### Gate 3 - Portable v2 Seam

Goal: make portability real without breaking v1.

Required:

- host-neutral semantic contract
- API version and structure-size fields
- no `HWND`, `HDC`, `HINSTANCE`, registry, AppKit, X11, SDL, or install logic
  in portable contracts
- surface, presenter, renderer, compute, and host capability separation
- v1 compatibility adapter
- renderer aliases preserved: `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, `null`

Exit:

- Nocturne and Ricochet use the v2 seam through adapters
- Win32 `.scr` artifacts remain operational

### Gate 4 - VisualIntent And Pack Compiler

Goal: turn vague user input into safe data, not arbitrary native code.

Required:

- `VisualIntent` resolver over the draft authoring contract
- product-family resolver
- product-specific schemas
- safe pack compiler
- candidate generation
- deterministic captures for each candidate
- proof-qualified promotion workflow

Initial target:

- text prompt to Plasma pack or preset

Exit:

- text can generate bounded, reviewable, proof-backed data candidates
- no unrestricted generated C is involved

### Gate 5 - First AIDE Canary

Goal: let AIDE coordinate ScreenSave work without owning ScreenSave truth.

Required:

- external AIDE-side admission of the pinned ScreenSave bridge profile
- ScreenSave project adapter used as the command interface
- AIDE WorkUnit maps to a bounded pack-generation task
- ScreenSave validation and proof bundle emitted
- independent review recorded
- human promotion decision required

Forbidden:

- automatic merge
- compatibility claim changes
- platform contract edits
- renderer routing edits
- release publication

Exit:

- AIDE can record an evidence-backed ScreenSave development attempt without
  becoming product runtime or truth authority

### Gate 6 - Plasma v2

Goal: rebuild Plasma around direct product-owned semantics and proof.

Required stable slice:

- Classic interference field
- one radial or warped generator
- domain warp
- bounded feedback
- continuous, banded, and contour output
- four high-quality materials
- flat presentation
- restrained CRT/dither treatments
- software/GDI reference path
- one accelerated realization
- direct controls
- deterministic captures
- migration of stable legacy presets

Exit:

- Plasma stable surface is controlled directly, not primarily as a preset
  browser
- experimental features remain explicitly in Labs or experimental mode

### Gate 7 - Workbench

Goal: build the product-facing proof and authoring tool over the same runner.

Workspaces:

- Catalog
- Author
- Inspect
- Compare
- Profile
- Release

Required:

- `libsslab`
- headless runner integration
- capture viewer
- exact/tolerant/perceptual comparison
- requested/resolved/degraded routing display
- proof-bundle export
- resource/performance panels
- AIDE EvidencePacket export or bridge

Exit:

- Workbench and CI use the same proof implementation

### Gate 8 - Manager

Goal: make the optional user companion real.

Required:

- catalog browsing
- preview
- install and repair
- configuration
- pack management
- provenance display
- rollback and safe cache
- current-user install discipline

Exit:

- Manager improves usability but is not required by autonomous saver artifacts

### Gate 9 - Preservation And Compatibility Certification

Goal: turn target language into evidence.

Required:

- preservation x86 ANSI build profile
- current x86/x64 profiles
- artifact audit gates
- runtime VM/hardware test matrix
- dependency allowlists
- known-limits ledger
- proof bundles per artifact profile
- release evidence directory

Exit:

- public compatibility claims are certified or explicitly lower-classed

### Gate 10 - Native Platform Expansion

Goal: add platforms without collapsing native identity.

Order per platform:

1. headless reference build
2. development runner
3. native presenter
4. native saver host
5. lifecycle evidence
6. packaging evidence
7. optional acceleration

Likely order:

- Windows preservation/current hardening first
- macOS `.saver` after proof kernel and v2 seam
- Linux XScreenSaver/standalone runner after native-host contract stabilizes
- WebAssembly preview only as gallery/docs/shareable preview, not as product
  replacement

### Gate 11 - New Core Release

Goal: ship the next real verified Core.

Required:

- curated Core membership
- proof bundles
- compatibility matrix
- artifact profile build outputs
- release notes
- checksums
- generated manifests
- known limitations
- rollback path

Exit:

- public release wording is backed by stored evidence

## AIDE Plan

Detailed supporting plan:
[aide-to-screensave-integration-plan.md](./aide-to-screensave-integration-plan.md).

AIDE should remain:

- external
- optional
- pinned
- removable
- report-only until proof is stronger
- absent from saver binaries

AIDE may own:

- WorkUnit records
- worker attempt history
- execution receipts
- evidence indexing
- development policy
- promotion proposals

ScreenSave must own:

- product identity
- visual semantics
- build graph
- artifact profiles
- compatibility status
- proof results
- final artistic acceptance

The ScreenSave/AIDE integration path is:

```text
ScreenSave project adapter
  -> ScreenSave validation and proof commands
  -> ScreenSave proof bundles
  -> AIDE EvidencePackets
  -> human promotion decision
```

Not:

```text
AIDE invents ScreenSave product truth.
```

## What We Should Not Do Next

Do not:

- start unrestricted text-to-C saver generation
- migrate all savers to v2 at once
- build Workbench before the runner/library is stable
- add new public renderer/API families before proof pressure requires them
- call broad Windows support certified without VM/hardware evidence
- make AIDE mandatory
- allow automatic merge for visual/product work
- turn ScreenSave into a generic graphics middleware empire
- add runtime executable plugin systems as the default modding path
- promote experimental Plasma features because they compile

## Immediate Next Packets

The next bounded implementation packets should be:

1. `proof: add minimal lifecycle smoke for Nocturne`
2. `proof: extract reusable libsslab boundary from canary-grade primitives`
3. `proof: add Ricochet fixed-step canary`
4. `contracts: define first product-specific authoring schema slice`
5. `packc: introduce safe pack compiler skeleton`
6. `visual-intent: add resolver prototype that emits bounded candidates`
7. `workbench: extract libsslab boundary`
8. `aide: add EvidencePacket export from project adapter receipts`

Each packet should state:

- goal
- current facts
- allowed paths
- forbidden paths
- invariants
- implementation outputs
- validation commands
- evidence outputs
- acceptance criteria
- known limitations
- rollback method

## Final End State

The complete ScreenSave end state is:

```text
ScreenSave creates durable native generative artworks.

Each saver owns its visual meaning.
The platform shares only proven mechanics.
Every product has a deterministic reference path.
Every release claim has machine-readable evidence.
Content is extended through safe validated data.
Manager and Workbench remain optional.
AIDE coordinates development but never becomes the product.
```

When this is complete, a user can download native standalone savers and never
care about AIDE, Workbench, Manager, or the SDK.

A maintainer can use Workbench, CI, and AIDE to author, inspect, validate,
compare, profile, and release with durable proof.

A contributor can use the SDK and pack compiler to create safe content or a
new product-local semantic core without redefining the platform.

A preservation user can see exactly which artifacts are certified, which are
binary-audited, which are buildable, which are targeted, and which are
experimental.

That is the full target:

```text
Autonomous native savers for users.
Deterministic proof for maintainers.
Validated data extension for creators.
Optional automation for development.
```
