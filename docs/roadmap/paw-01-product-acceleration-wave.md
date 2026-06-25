# PAW-01 Product Acceleration Wave

Status: active umbrella program; Gate A closed and Gate B
`proof-kernel-v1-ricochet` is active.

## Purpose

`PAW-01` is the next ScreenSave execution program. It compresses the current
vision into one gated product-acceleration wave:

```text
Native build truth
-> generic proof library
-> Ricochet second canary
-> portable v2 seam
-> Plasma v2
-> pack compiler and VisualIntent
-> Workbench
-> Manager integration
-> verified creator/product preview
```

AIDE may coordinate WorkUnits and evidence. ScreenSave owns product meaning,
execution, proof, compatibility status, and promotion.

## PAW-00 Reconciliation Checkpoint

Complete. The repository closed the truth drift created by recent
proof-kernel implementation work.

Required checkpoint outputs:

- `PROJECT_STATE.toml` records compiled product-session proof, canonical RGBA8
  hashing, lifecycle evidence, non-vacuous PE audit JSON, and CI proof lanes.
- Proof Kernel v0 docs distinguish achieved v0 behavior from future v1 work.
- The ultimate project report points at `PAW-01` rather than older disconnected
  next packets.
- `check_proof_kernel.py` recomputes committed implementation digests and
  refuses stale proof-relevant baseline metadata.

## Gate A - Native Build And Artifact Truth

Status: closed.

Objective: a clean Windows build produces an exact, profile-bound artifact set
and a complete proof receipt.

Deliverables:

- `catalog/artifact_sets.toml`
- `catalog/proof_profiles.toml`
- `tools/artifactmanifest/`
- exact expected artifact manifests per profile
- profile-aware PE audit over manifests, not loose roots
- fixed build commands such as `screensave.build.windows-current-x86`
- native Windows CI for VS2022 Release Win32
- Proof Bundle v1 with separate execution, capture, comparison, lifecycle,
  performance, artifact-audit, compatibility, artistic-review, and
  release-promotion axes

Exit:

- clean Windows checkout builds all expected current artifacts
- generated artifact manifest has no missing or unexpected binaries
- PE audit is nonempty and profile-aware
- adapter proof runs against built binaries
- proof bundle integrity validates
- Proof Kernel v0 is formally closed

Closeout evidence:

- clean local `windows-current-x86` receipt at source
  `cb77a40166325f80b834d33ab61d290ab1d4aeb9`
- clean local `windows-current-tools` receipt at the same source revision
- adapter proof and normalized Proof Bundle v1 closeout evidence
- hosted Windows CI configured to regenerate native build/audit, adapter proof,
  and normalized Proof Bundle v1 outputs after building

## Gate B - libsslab And Ricochet

Status: active.

Objective: replace the Nocturne-specific proof runner with reusable compiled
proof infrastructure and a second structurally different canary.

Deliverables:

- private `libsslab` ABI v0 header is present
- shared RGBA8 renderer and capture helpers are present
- generated product proof registry from the catalog is present
- catalog-owned Nocturne and Ricochet proof profiles are present
- Ricochet deterministic proof canary is present
- lifecycle, profile, and short-soak surfaces are present
- exact deterministic proof for Nocturne and Ricochet is present
- profile-driven Proof Bundle v1 composition is present
- CI runs the Nocturne and Ricochet proof/profile matrix
- a minimal BenchLab/Workbench shell binds to `sslab` and the generated proof registry
- private static `libsslab` ABI v0 core now owns context, product, and session execution
- Nocturne and Ricochet have product adapters inside the `libsslab` tranche
- Workbench smoke executes both canaries through in-process `libsslab` calls
- capture, lifecycle, profile, and short-soak receipt operations are implemented in compiled C
- `sslab_runner proof --profile <key>` builds over one private static `libsslab`
  artifact and executes Nocturne and Ricochet profiles through the same command
  shape
- Ricochet has committed exact baselines for frames 0, 4, 8, and 32, and its
  proof profile is promoted to implemented with baseline references

Remaining Gate B extraction:

- remove remaining product-specific execution dispatch from `sslab.py` where
  practical
- keep portable v2 closed until this Gate B extraction is accepted

Exit:

- Nocturne and Ricochet run through one compiled proof library
- both have exact deterministic proof
- both have lifecycle evidence
- both have profile evidence
- Python contains no product semantics
- Workbench can link the same proof library

## Gate C - Portable v2 Seam

Objective: introduce the host-neutral shared semantic seam only after Nocturne
and Ricochet prove the mechanics.

Deliverables:

- `screensave/v2/types.h`
- `screensave/v2/product.h`
- `screensave/v2/session.h`
- `screensave/v2/surface.h`
- `screensave/v2/diagnostics.h`
- `screensave/v2/config.h`
- v1 Win32 compatibility adapter

Rules:

- versioned and size-prefixed structures
- opaque handles
- explicit ownership
- no `windows.h`
- no `HWND`, `HDC`, `HINSTANCE`, registry, native dialog, or installation
  concepts
- host, presenter, surface, renderer, and compute remain distinct

Exit:

- Nocturne and Ricochet use v2 semantic/session contracts
- Win32 `.scr` artifacts still use the preserved v1 host
- headless proof uses the same products
- no platform type appears in v2 headers

## Gate D - Plasma v2

Objective: make Plasma the proof-backed flagship saver by refining the U09
runtime rather than starting a parallel engine.

Stable slice:

- Classic interference field
- one additional proved field family
- domain warp
- bounded feedback
- continuous, banded, and contour output
- four curated materials
- flat presentation
- restrained dither and CRT treatment
- software reference realization
- GL11 accelerated realization
- direct Basic controls
- stable U09 preset migration

Experimental holdbacks:

- glyph output
- heightfield
- ribbon
- premium presentation
- journeys and transitions
- GL21, GL33, and GL46 stable promotion
- arbitrary graph authoring

Exit:

- Plasma v2 runs through the portable seam and `libsslab`
- U09 stable content migrates deterministically
- Basic controls materially affect output
- software and GL11 paths are honestly qualified
- experimental features remain segregated

## Gate E - Safe Creator Pipeline

Objective: enable Workbench and bounded generation through validated data, not
unrestricted native code.

Deliverables:

- `tools/packc/`
- `contracts/pack_v1.md`
- `contracts/plasma_spec_v2.md`
- deterministic VisualIntent-to-Plasma resolver
- fixed adapter capabilities for pack validation, Plasma pack compilation,
  VisualIntent resolution, and Plasma reference proof

Security rules:

- data only
- no scripts or executable payloads
- no absolute paths or traversal
- size, count, string, and asset limits
- provenance and license metadata
- quarantine on failure
- transactional activation

## Gate F - Workbench v1

Objective: create the creator and maintainer application over `libsslab`.

Initial product split:

```text
libsslab       authoritative execution/proof library
sslab          CLI frontend
benchlab       retained regression/diagnostics frontend
workbench      new product-facing frontend
```

Workspaces:

- Catalog
- Author
- Inspect
- Compare
- Profile
- Release

Exit:

- Workbench and CI call the same `libsslab` functions
- Nocturne, Ricochet, and Plasma run in Workbench
- Plasma packs can be authored and proved
- captures can be inspected and compared
- profile and lifecycle evidence are visible
- release proof bundles can be exported

## Gate G - Manager, AIDE, And Preview

Objective: integrate product management and evidence coordination without
making either mandatory for saver binaries.

Manager evolves from the current Suite lineage:

- pack installation and removal
- content-addressed cache
- provenance display
- artifact verification
- repair
- rollback
- current-user installation discipline

AIDE remains optional development infrastructure:

- fixed ScreenSave capability admission
- WorkUnits
- execution receipts
- EvidencePacket export
- proof indexing
- review records
- promotion proposals

No autonomous source mutation is admitted in `PAW-01`.

## Parallel Execution Map

```text
PAW-00 Truth reconciliation
          |
          v
PAW-A Native build and artifact truth
          |
          +-------------------------------+
          |                               |
          v                               v
PAW-B libsslab + Ricochet          Pack schemas + Workbench shell
          |
          v
PAW-C Portable v2 seam
          |
          +-------------------------------+
          |                               |
          v                               v
PAW-D Plasma v2                  Workbench live integration
          |                               |
          +---------------+---------------+
                          v
                packc + VisualIntent
                          |
                          v
               Manager + AIDE evidence
                          |
                          v
                  Product preview
```

## Focused Commit Sequence

1. `state: reconcile achieved Proof Kernel v0 truth`
2. `proof: verify committed implementation digests`
3. `catalog: define exact artifact sets and proof profiles`
4. `build: add fixed current-Windows build controller`
5. `ci: build audit and prove Win32 Release`
6. `contracts: implement proof bundle v1`
7. `proof: extract generic libsslab`
8. `proof: generate product proof registry from catalog`
9. `proof: add Ricochet deterministic canary`
10. `proof: add lifecycle profile and soak surfaces`
11. `contracts: introduce portable v2 semantic seam`
12. `runtime: adapt Nocturne and Ricochet through v2`
13. `plasma: define v2 specification and migration`
14. `plasma: implement stable software reference slice`
15. `plasma: qualify GL11 realization`
16. `packc: compile bounded Plasma packs`
17. `visual-intent: emit proved Plasma candidates`
18. `workbench: add Catalog Inspect Compare and Profile`
19. `workbench: add Plasma Author and Release workspaces`
20. `manager: add pack provenance repair and rollback`
21. `aide: export ScreenSave proof EvidencePackets`
22. `release: assemble verified product preview`

## Exclusions

`PAW-01` does not include:

- migration of all nineteen savers to v2
- D3D12, Vulkan, or additional renderer-family work
- stable promotion of GL21, GL33, or GL46
- arbitrary shader or node-graph authoring
- native macOS or Linux saver hosts
- preservation certification
- autonomous source-editing agents
- automatic merge or release
- plugin marketplace work
- broad public compatibility expansion

Those belong to the later Core Completion and Native Expansion wave.

## End State

At completion:

```text
Windows artifacts build and audit exactly.
Nocturne and Ricochet prove the shared architecture.
Portable v2 has multiple real consumers.
Plasma v2 is a real proof-backed flagship saver.
packc provides bounded data extension.
VisualIntent produces reviewable candidates.
Workbench authors, runs, inspects, compares, profiles, and proves.
Manager installs and manages packs without becoming mandatory.
AIDE records evidence without owning product truth.
```
