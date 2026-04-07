# Backlog Routing

This document records the `C11` backlog and routing model used during the completed `C11` through `C16` bridge.
It is historical context rather than the active post-`C16` execution plan.

Use it to understand how earlier continuation work was routed across savers, shared layers, renderer discussions, release-channel follow-on work, and the parked future-ideas pool.
Active post-`C16` sequencing now lives in `docs/roadmap/post-c16-program.md`.

## Routing Categories

| Category | Route To | Use For | Do Not Use For |
| --- | --- | --- | --- |
| Saver-local polish | Owning saver subtree and saver-local notes | Preset curation, config wording, pacing, theme balance, pack growth, long-run tuning for one saver | Shared settings contracts, renderer policy, build-lane or installer decisions |
| Shared-platform/runtime | `platform/`, shared specs, and cross-cutting validation notes | Stable reusable contracts, shared host/runtime behavior, shared config behavior, shared diagnostics helpers | One product's private behavior or art direction |
| Renderer-tier work | Renderer docs, renderer validation, later renderer R&D notes | `gdi` / `gl11` / `gl21` parity, fallback truth, tier-specific evidence, later `gl33` / `gl46` research | Product-local preset or pack tuning |
| Suite/meta layer | `suite`, `anthology`, and related app/meta docs | Cross-saver browse, launch, orchestration, suite metadata visibility, meta-saver selection logic | BenchLab diagnostics-only work or installer behavior |
| Release/distribution | Core, installer, extras, release notes, and delivery validation | Core refresh, installer follow-on, extras gating, uninstall/repair policy, release readiness | Saver-local rendering or config UX |
| Docs/SDK/tooling | `docs/`, `products/sdk/`, `tools/`, and validation helpers | Contributor docs, examples, backlog notes, static validators, truth-bearing repo status | Shipping runtime behavior |
| Validation/hardening | `validation/`, smoke checks, and bounded diagnostics work | Lifecycle evidence, persistence checks, artifact truth, long-run verification, compatibility notes | Unbounded new feature work |
| Deferred/parked ideas | Parked backlog entries and concept pool | Working-title future savers, speculative renderer R&D, later release goals not ready for active waves | Current implementation promises |

## Required Backlog Fields

Every active backlog item should stay compact and include these fields:

| Field | Allowed Values | Meaning |
| --- | --- | --- |
| `priority` | `P1`, `P2`, `P3`, `Parked` | `P1` blocks the next structured phase, `P2` is important but not blocking, `P3` is useful but optional, `Parked` is not on the active execution line |
| `effort` | `S`, `M`, `L` | Small, medium, or large implementation/planning weight |
| `risk` | `low`, `medium`, `high` | Risk of compatibility churn, architectural drift, or review complexity |
| `depends_on` | backlog IDs or `none` | Shared gating item or prior work bucket |
| `target_bucket` | `C12-X1`, `C12-X2`, `Wave-A`, `Wave-B`, `Wave-C`, `RND-R`, `REL-R`, `PARKED` | Where the work should land next |
| `owner_area` | `saver:<slug>`, `platform`, `renderer`, `suite`, `anthology`, `benchlab`, `distribution`, `sdk`, `docs`, `validation` | Which part of the repo owns the decision |
| `notes` | short free text | Why the item exists or what should stay bounded |

Avoid pseudo-precision.
Do not add percentages, point scores, or issue-tracker ceremony unless a later repo workflow genuinely needs it.

## Historical Execution Buckets

| Bucket | Scope | Exit Goal |
| --- | --- | --- |
| `C12-X1` | Cross-cutting polish for config, metadata, suite visibility, docs truth, and validation wording | Shared rough edges are reduced before saver-local wave work fans out |
| `C12-X2` | Cross-cutting lifecycle, renderer, and release-evidence cleanup | Validation and release-facing truth are coherent before broader rerelease work |
| `Wave-A` | First saver-specific polish wave | `nocturne`, `ricochet`, `deepfield`, `plasma`, and `phosphor` as the first bounded premium-polish set after `C12` |
| `Wave-B` | Second saver-specific polish wave | Implemented for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A |
| `Wave-C` | Heavyweight, showcase, and meta-saver refinement | The final bounded saver-polish pass for the remaining scenic, renderer-showcase, and anthology products with higher coordination cost |
| `RND-R` | Later renderer or platform research | Work that should stay deferred until the polish line is stable |
| `REL-R` | Later release/distribution follow-on | Delivery work that belongs after the active polish waves |
| `PARKED` | Idea pool only | Not on the current execution line |

## Category Overlap Rules

- Route a task to saver-local polish unless at least two real products need the same stable change.
- Promote an item to shared-platform/runtime only when reuse is proven and the shared contract can stay small.
- Route renderer concerns to renderer-tier work even when the symptom first appears in one saver.
- Route bundle, installer, uninstall, and release-notes follow-ons to release/distribution even if they mention one saver by name.
- Route authoring guidance, templates, and static checkers to docs/SDK/tooling, not to product or platform backlogs.
- When one item touches both a saver and a shared area, split it into a shared gating item plus a saver-local follow-on item.

## Wave Assignment Rules

- `C12-X1` and `C12-X2` happen before any saver-local wave enters active implementation.
- `Wave-A` starts with lower-risk products that should not force platform renegotiation; the first real `C13` pass covers `nocturne`, `ricochet`, `deepfield`, `plasma`, and `phosphor`.
- `Wave-B` handled `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A and did not reopen suite-wide vocabulary or platform law.
- `Wave-C` handled `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` without reopening suite-wide vocabulary or the shared platform contract.
- `C14` final rerelease hardening is complete and preserved the release-candidate baseline.
- `C15` release doctrine and channel split are complete.
- `C16` Core release refresh and baseline freeze is complete.
- Any post-`C16` release/distribution follow-on now routes through the short `SS` bridge first and only reaches `PL` after `SS` closes.
- `RND-R` and `REL-R` stay deferred unless the active polish line proves they are unblockers.

## Canonical Ownership Summary

- `master` coordination owns backlog ordering, wave admission, and any shared decision that changes repo truth.
- Saver-local owners own product meaning, presets, themes, packs, and product-specific polish items.
- Shared owners own platform/runtime, renderer, release/distribution, and docs/SDK/tooling buckets only when the routing rules justify it.
