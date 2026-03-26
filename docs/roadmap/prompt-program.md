# Prompt Program

This document defines the intended Series 00 through 17 prompt program.
It is explanatory, but later series should treat it as the default execution map unless a newer roadmap update says otherwise.

| Series | Purpose | Outputs | Dependencies | Allowed To Modify | Must Preserve |
| --- | --- | --- | --- | --- | --- |
| S00 | Constitutional foundation and control plane | Specs, docs, AGENTS hierarchy, `.codex` config, directory skeleton | None | Repo structure, docs, specs, control plane | No runtime implementation, compatibility baseline |
| S01 | Governance, community-health, and CI foundation | Issue forms, PR template, CODEOWNERS, contribution guide, stage-appropriate workflows, lightweight repo-validation scripts | S00 | `.github/`, `tools/scripts/`, `CONTRIBUTING.md`, targeted docs and changelog updates | Constitutional specs, no runtime or packaging implementation |
| S02 | Build scaffold and toolchain lane foundation | Build philosophy, concrete VS2022 and MinGW i686 lanes, honest VS6/VS2008 scaffold, build-layout validation | S01 | `build/`, `tools/scripts/`, minimal build-only stubs under `platform/` and product trees, targeted docs and changelog updates | Constitutional specs, no real runtime or packaging implementation |
| S03 | Public platform surface, Win32 host groundwork, and initial GDI floor | Initial public headers, platform boundary cleanup, classic mode-routing groundwork, and first universal renderer-floor scaffolding | S02 | `platform/`, `specs/`, `docs/`, `tests/`, `validation/` | C89 baseline, ANSI host path, product isolation |
| S04 | Shared config resolution and saver lifecycle wiring | Config loading path, preset/theme resolution scaffolding, lifecycle contracts tightened | S02-S03 | `platform/`, `specs/`, `tests/`, docs | Product-local ownership, deterministic seed rules |
| S05 | SDK and product template groundwork | Template saver skeleton, SDK notes, product manifest conventions, saver template docs | S00-S04 | `products/sdk/`, `products/savers/_template/`, `specs/`, docs | No overgrown platform API, no fake sample outputs |
| S06 | `nocturne` first saver product | First releasable saver path, product defaults, presets, validation notes | S03-S05 | `products/savers/nocturne/`, shared platform only if justified, tests, validation | Universal baseline first, product isolation |
| S07 | BenchLab and validation instrumentation | `benchlab` app skeleton, measurement hooks, validation workflow refinement | S03-S06 | `products/apps/benchlab/`, `tests/`, `validation/`, `tools/`, narrow platform hooks | No runtime dependency on BenchLab |
| S08 | Compatibility matrix and capture discipline | OS matrix notes, VM conventions, smoke coverage, evidence capture rules | S02-S07 | `tests/`, `validation/`, `docs/`, narrow tooling | No fake evidence, no unsupported compatibility claims |
| S09 | Gallery and Player shell apps | `gallery` and `player` app skeletons for browsing and running products | S05-S08 | `products/apps/gallery/`, `products/apps/player/`, platform, docs, tests | Saver baseline remains `.scr`, apps stay optional |
| S10 | `ember` and `oscilloscope_dreams` saver set | Two more savers with shared lessons folded back into the platform only where reused | S03-S09 | Product subtrees, narrow platform/shared docs, tests, validation | Keep shared changes justified and reusable |
| S11 | Quiet and classic saver pack | Additional low-cost ambient or classic savers using the universal baseline | S03-S10 | `products/savers/`, tests, validation, narrow platform | GDI compatibility and small shared surface |
| S12 | Mathematical and procedural saver pack | Geometry, waveform, or rules-based savers with deterministic behavior | S03-S11 | `products/savers/`, tests, validation, narrow platform | Deterministic seeds, long-run stability |
| S13 | Scenic and atmospheric saver pack | Higher-content scenic savers and supporting shared assets | S03-S12 | `products/savers/`, `assets/`, tests, validation, narrow platform | No mandatory higher renderer tier |
| S14 | Systemic and simulation saver pack | Longer-running systems or simulation-style savers and soak validation | S03-S13 | `products/savers/`, `tests/soak/`, `validation/`, narrow platform | Stability over long unattended runs |
| S15 | `gl_plus` backend and accelerated gallery paths | Optional advanced renderer tier, capability gating, accelerated showcase paths | S03-S14 | `platform/src/render/gl_plus/`, platform contracts, tests, validation, docs | GDI universal baseline, GL11 optional tier remains intact |
| S16 | Packaging and release pipeline material | Installer and portable packaging docs, release note flow, product metadata hardening | S06-S15 | `packaging/`, `docs/`, `validation/`, `products/`, narrow tooling | No runtime logic in packaging, truthful release evidence |
| S17 | Release hardening and suite handoff | Final compatibility sweep, SDK examples, suite-level release readiness, cleanup | S06-S16 | Whole repo as needed for stabilization | Preserve all normative contracts and recorded evidence |

## Program Rules

- Earlier series establish the floor that later series must preserve.
- Shared APIs should only grow when multiple products need them.
- Product series may update specs and docs when the repository truth changes, but they must do so explicitly.
- Validation evidence should accumulate alongside implementation maturity.
