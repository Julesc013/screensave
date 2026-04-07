# Refinement Thread Model

This note records how refinement work was organized after `C11` during the completed continuation bridge.
It is historical context rather than the active post-`C16` execution model.

The goal is to let saver-local work proceed in parallel without letting repo truth fork.
Active post-`C16` sequencing now lives in `docs/roadmap/post-c16-program.md` and `docs/roadmap/sx-series.md`.

## Thread Types

### Master / Orchestration Thread

Owns:

- backlog ordering
- wave admission
- shared decision arbitration
- changes to roadmap truth, validation baselines, and release sequencing

Required behavior:

- split shared and saver-local work before implementation starts
- reject saver-local shortcuts that silently change platform or renderer contracts
- merge canonical decisions back into repo docs, specs, and changelog entries

### Shared-Platform Thread

Owns:

- reusable runtime changes
- shared settings or pack-contract refinements
- host/runtime lifecycle changes that affect more than one product
- changes to public or semi-public platform contracts

Rules:

- do not absorb product-local behavior just because one saver wants it
- feed all contract changes back through the master thread before they are treated as canonical

### Renderer Thread

Owns:

- renderer-tier validation
- fallback messaging
- capability evidence and tier-specific polish
- later renderer R&D notes that are not yet active work

Rules:

- keep `gdi` mandatory
- keep `gl11` and `gl21` optional and evidence-based
- keep `gl33` and `gl46` parked until a later explicit decision reopens them

### Release / Distribution Thread

Owns:

- Core refresh and freeze work
- installer channel refresh
- extras gating and release-evidence follow-on work
- uninstall / repair / release-evidence follow-on work

Rules:

- do not redefine runtime behavior
- do not turn release work into silent product changes
- keep Core, Installer, Suite, BenchLab, SDK, and Extras as distinct channels

### Suite / Meta Coordination Thread

Owns:

- `suite` browser/launcher/config follow-on work
- `anthology` orchestration follow-on work
- role-boundary enforcement between `suite`, `anthology`, and BenchLab

Rules:

- `suite` remains the browser/launcher/config surface
- `anthology` remains the bounded cross-saver `.scr`
- BenchLab remains diagnostics-only

### Per-Saver Refinement Threads

Create one thread per saver only when that saver enters an active wave.

Each saver thread owns:

- preset curation
- theme and pack growth
- product-local config wording
- product-local pacing, tuning, and polish
- product-local validation updates

Each saver thread does not own:

- shared settings contracts
- renderer-tier law
- build-lane architecture
- release policy

## Decision Return Path

When a saver thread uncovers a shared issue:

1. Record the product-local symptom in the saver thread.
2. Route the shared concern back to the master thread with the relevant backlog item or a proposed new routed item.
3. Update canonical repo truth only in the shared/master lane.
4. Return the approved shared decision to the saver thread as an input, not as a parallel fork.

## Thread Start Rules

Start a new refinement thread only when:

- the product has an active backlog item in `docs/roadmap/backlog-inventory.md`
- the target bucket is active or explicitly unblocked
- the expected write scope is narrow enough to review cleanly

Do not open a saver-local refinement thread for parked concept work.
Do not reuse the old `C12`, `C13`, or `REL-R` bucket names as active post-`C16` program labels without first mapping the work onto `SX` or `PL`.
Only explicitly release-closure or maintenance-policy work belongs back in the reserved `SS` sidecar.

## Thread Close Rules

Close a refinement thread when:

- its routed backlog items are complete or explicitly deferred
- any shared decisions have been merged back into canonical repo docs/specs
- the saver-local docs and validation notes match the implemented result
