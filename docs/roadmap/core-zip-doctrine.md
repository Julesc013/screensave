# Core ZIP Doctrine

This document defines what `ScreenSave Core` means and how Core inclusion is decided.

## What Core Is

- A ZIP of release-grade standalone `.scr` saver products.
- The primary product for normal users.
- The best standalone saver experience the repo can honestly support at release time.
- A saver-first artifact that may include `anthology` only when `anthology` meets the same bar as every other Core saver.
- The durable release anchor that later `SS` and `PL` work must preserve rather than quietly redefine.

## What Core Is Not

- Not the installer.
- Not the `suite` app.
- Not BenchLab.
- Not the SDK.
- Not a catch-all bundle for unstable material.
- Not a claim that every saver in source control must ship in the same Core freeze.

## Core Inclusion Gate

A saver is eligible for Core only when all of the following are true.

| Gate | Required Condition | Minimum Honest Evidence |
| --- | --- | --- |
| Standalone identity | The saver is a real standalone `.scr` product with canonical naming, manifest identity, and an intended checked-in target | saver manifest, build target, and source-controlled product docs |
| Host lifecycle | The saver behaves coherently on the real Windows screensaver path | validation notes that cover startup, shutdown, and the claimed host modes |
| `Settings...` path | The saver has a coherent Windows picker/config handoff and a coherent product-owned settings path when it claims configurability | product docs plus validation evidence or explicit limitation notes |
| Baseline renderer behavior | The saver has acceptable `gdi` baseline behavior and does not depend on optional tiers just to be usable | validation evidence and truthful compatibility notes |
| Curated content | Presets, themes, packs, and defaults are curated enough to represent the product well in a public saver ZIP | product-owned content plus release-facing notes |
| Metadata and versioning | Product metadata, resources, and visible version identity are coherent enough for release packaging | manifest/version alignment and release-note coverage |
| Known limitations | Remaining limitations are recorded honestly where release work can see them | validation note, known-issues note, or release note |
| Release polish | The saver is polished enough to represent ScreenSave Core rather than merely existing in source | review against this gate plus the release notes and validation notes |
| Companion independence | The saver does not require `suite`, BenchLab, SDK material, or unstable extras to function as a standalone saver | packaging docs and product docs |

## Core Exclusion Rules

Hold a saver or related content out of Core when any of these are true:

- it is source-present but not ready for honest artifact claims
- its Windows picker or `Settings...` path is not coherent enough for release use
- its `gdi` floor is weak enough that the standalone saver experience is not acceptable
- its presets, themes, or packs still read as experimental rather than curated
- its limitations are not documented clearly enough to ship responsibly
- it depends on companion tooling to feel complete

When material is interesting but not yet ready for Core, prefer `Extras` over silent bundle creep.

## Companion And Support Rules

- The Installer channel packages Core more conveniently; it does not decide what Core is.
- The `suite` app may browse, launch, preview, and hand off saver settings, but Core cannot depend on it.
- BenchLab is a developer and advanced-user diagnostics harness and is never part of Core by default.
- SDK material is contributor-facing and is never part of Core by default.
- Experimental packs or lower-confidence showcase content belong in Extras, not in Core.

## Evidence Rule

Source presence is enough to keep a product in the repo.
Source presence alone is not enough to claim that the product belongs in a frozen Core ZIP.
Core claims must be backed by the release notes, validation notes, and artifact reality available at the time of the freeze.

## C16 Outcome

- `C15` defined the doctrine and the gate.
- `C16` applied that gate to the current saver line.
- The frozen per-saver decision lives in `validation/notes/c16-core-inclusion-matrix.md`.
- The frozen baseline note lives in `validation/notes/c16-release-baseline.md`.
- The continuation bridge is closed after `C16`.
- Post-`C16` work now proceeds as `SS` first and `PL` second from the frozen Core baseline; see `docs/roadmap/post-c16-program.md`.
