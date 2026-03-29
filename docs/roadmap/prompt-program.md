# Prompt Program

This document defines the intended Series 00 through 17 prompt program.
It is explanatory, but later series should treat it as the default execution map unless a newer roadmap update says otherwise.

| Series | Purpose | Outputs | Dependencies | Allowed To Modify | Must Preserve |
| --- | --- | --- | --- | --- | --- |
| S00 | Constitutional foundation and control plane | Specs, docs, AGENTS hierarchy, `.codex` config, directory skeleton | None | Repo structure, docs, specs, control plane | No runtime implementation, compatibility baseline |
| S01 | Governance, community-health, and CI foundation | Issue forms, PR template, CODEOWNERS, contribution guide, stage-appropriate workflows, lightweight repo-validation scripts | S00 | `.github/`, `tools/scripts/`, `CONTRIBUTING.md`, targeted docs and changelog updates | Constitutional specs, no runtime or packaging implementation |
| S02 | Build scaffold and toolchain lane foundation | Build philosophy, concrete VS2022 and MinGW i686 lanes, honest VS6/VS2008 scaffold, build-layout validation | S01 | `build/`, `tools/scripts/`, minimal build-only stubs under `platform/` and product trees, targeted docs and changelog updates | Constitutional specs, no real runtime or packaging implementation |
| S03 | Win32 `.scr` host skeleton and classic lifecycle groundwork | Real screen/preview/config dispatch, full-screen and preview child-window paths, a small configuration dialog, provisional settings persistence, and a host-local placeholder visual path | S02 | `platform/`, `build/`, targeted docs, and changelog updates | C89 baseline, ANSI host path, no reusable renderer yet, product isolation |
| S04 | Shared core runtime and first public platform API layer | Narrow public headers, reusable core runtime modules, deterministic RNG/timing/diagnostics/config services, renderer and saver contracts, and light host adoption | S02-S03 | `platform/`, `build/`, targeted docs, and changelog updates | Product-local ownership, deterministic seed rules |
| S05 | Mandatory GDI backend and shared present path | Reusable GDI renderer backend, offscreen-buffer present path, host integration, baseline primitives/bitmap support, and a tiny validation scene | S00-S04 | `platform/`, `build/`, targeted docs, and changelog updates | Compatibility-first renderer floor, no real product yet |
| S06 | `nocturne` first saver product | First releasable saver path, product defaults, presets, validation notes | S03-S05 | `products/savers/nocturne/`, shared platform only if justified, tests, validation | Universal baseline first, product isolation |
| S07 | BenchLab and validation instrumentation | `benchlab` app skeleton, measurement hooks, validation workflow refinement | S03-S06 | `products/apps/benchlab/`, `tests/`, `validation/`, `tools/`, narrow platform hooks | No runtime dependency on BenchLab |
| S08 | Optional GL11 backend and runtime selection | Conservative GL11 backend, capability capture, renderer selection, explicit fallback, BenchLab validation | S03-S07 | `platform/`, `products/apps/benchlab/`, `build/`, targeted docs, tests | GDI remains mandatory, no higher GL tier, no product-level GL dependency |
| S09 | Motion Family with `ricochet` and `deepfield` | First multi-product saver family on the dual-backend platform | S03-S08 | `products/savers/`, narrow platform/build/docs updates, tests | Shared changes must stay justified and reusable |
| S10 | Framebuffer and vector family with `ember` and `oscilloscope_dreams` | Product-local framebuffer and vector savers with disciplined shared renderer use | S03-S09 | `products/savers/`, narrow platform/build/docs updates, tests | Baseline renderer floor remains intact |
| S11 | Grid and simulation family with `pipeworks` and `lifeforms` | Stateful grid-driven saver family with narrow shared helpers only where reused | S03-S10 | `products/savers/`, narrow platform/build/docs updates, tests, validation | No accidental general simulation framework |
| S12 | Systems and ambient family with `signal_lab`, `mechanical_dreams`, and `ecosystems` | Layered composition, ambient behavior, and modest system-interface savers | S03-S11 | `products/savers/`, narrow platform/build/docs updates, tests, validation | No accidental player, gallery, or framework scope |
| S13 | Places and atmosphere family | Scenic and atmospheric savers built on the mature multi-product, dual-backend platform | S03-S12 | `products/savers/`, `assets/`, tests, validation, narrow platform | No mandatory higher renderer tier |
| S14 | Systemic and simulation saver pack | Longer-running systems or simulation-style savers and soak validation | S03-S13 | `products/savers/`, `tests/soak/`, `validation/`, narrow platform | Stability over long unattended runs |
| S15 | `gl21` backend and accelerated gallery paths | Optional advanced renderer tier, explicit higher-tier GL placeholders, accelerated showcase paths | S03-S14 | `platform/src/render/gl21/`, `platform/src/render/gl33/`, `platform/src/render/gl46/`, `platform/src/render/null/`, platform contracts, tests, validation, docs | GDI universal baseline, GL11 optional tier remains intact |
| S16 | Packaging and release pipeline material | Installer and portable packaging docs, release note flow, product metadata hardening | S06-S15 | `packaging/`, `docs/`, `validation/`, `products/`, narrow tooling | No runtime logic in packaging, truthful release evidence |
| S17 | Release hardening and suite handoff | Final compatibility sweep, SDK examples, suite-level release readiness, cleanup | S06-S16 | Whole repo as needed for stabilization | Preserve all normative contracts and recorded evidence |

## Program Rules

- Earlier series establish the floor that later series must preserve.
- Shared APIs should only grow when multiple products need them.
- Product series may update specs and docs when the repository truth changes, but they must do so explicitly.
- Validation evidence should accumulate alongside implementation maturity.
