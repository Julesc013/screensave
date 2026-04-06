# C15 Channel Matrix

This note records the `C15` release-doctrine pass layered on top of the live `C14` baseline.

## Preconditions Confirmed

- `CHANGELOG.md` already recorded the implemented continuation line through `C14`.
- `docs/roadmap/post-s15-plan.md` already documented the post-`S15` baseline through the `C14` release-candidate pass.
- The live repo already documented the current saver line, `anthology`, `suite`, BenchLab, the SDK surface, the portable-bundle architecture, and the installer architecture as real surfaces.
- `packaging/release_notes/release-candidate-notes.md` and the `validation/notes/c14-*.md` notes already recorded the release-candidate state before `C15` clarified the enduring artifact channels.

## Channel-Confusion Inventory

| Area | Status | Observation | C15 Action |
| --- | --- | --- | --- |
| product role docs | already correct | `suite`, `anthology`, BenchLab, and SDK role boundaries were already explicit in product docs | preserved |
| packaging framing | stale | portable and installer surfaces were described operationally, but the repo did not name one primary product above the others | defined `Core` as primary and `Installer` as a companion |
| root and roadmap docs | missing doctrine | the repo named products and artifacts but did not define the primary end-user release channel or the next-step freeze | added explicit doctrine docs and `C16` handoff wording |
| release-candidate notes | misleading by omission | the `C14` notes described the candidate payloads but did not state how they map onto a longer-lived release model | added Core-candidate and companion-channel interpretation |
| extras policy | missing | unstable or lower-confidence material had no single named holdback channel | formalized `Extras` as the holdback channel |

## Canonical Channel Matrix

| Channel | Audience | Primary Or Companion | Current C15 Interpretation | Source Of Truth |
| --- | --- | --- | --- | --- |
| `Core` | normal end users | primary | the current `C14` portable bundle is the pre-freeze Core candidate surface | `docs/roadmap/core-zip-doctrine.md` |
| `Installer` | end users who want convenience | companion | the current `C14` installer package is the optional installed-distribution companion built from the Core candidate | `packaging/installer/README.md` |
| `Suite` | end users who want browse and launch help | companion | source-complete companion app outside Core | `products/apps/suite/README.md` |
| `BenchLab` | developers and advanced users | companion | diagnostics-only harness outside Core | `products/apps/benchlab/README.md` |
| `SDK` | contributors | companion | contributor surface outside Core | `products/sdk/README.md` |
| `Extras` | opt-in experimenters | companion | defined now as the holdback channel for unstable or lower-confidence material; no frozen contents are claimed here | `docs/roadmap/release-channels.md` |

## Core Gate Summary

- Core is the primary product, not a generic bundle label.
- Core membership requires a real standalone `.scr`, coherent Windows picker and `Settings...` behavior, acceptable `gdi` baseline behavior, curated product content, coherent metadata, and documented limitations.
- Companion tools do not belong in Core by default.
- Lower-confidence material should be held back into Extras rather than stretched into Core by optimism.
- The full gate lives in `docs/roadmap/core-zip-doctrine.md`.

## C16 Handoff

- `C15` did not freeze the final Core lineup.
- `C16` is the next continuation prompt and will refresh the Core candidate surface and freeze the actual Core baseline.
