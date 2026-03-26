# AGENTS.md

This file applies to the whole repository unless a deeper `AGENTS.md` overrides it.

## Purpose

ScreenSave is a monorepo for a retro-compatible Windows screensaver platform and a family of separate products built on that platform.

## Directory Map

- `platform/` is shared runtime and public platform surface.
- `products/` is product-local code, manifests, presets, and content.
- `specs/` is normative project law.
- `docs/` is explanatory architecture and roadmap material.
- `tests/`, `validation/`, `tools/`, `build/`, and `packaging/` support verification and delivery.

## Invariants

- Preserve the C89 baseline.
- Preserve x86 Win32 `.scr` as the baseline saver artifact.
- Preserve the ANSI Win32 host path.
- Keep GDI mandatory and GL11 optional.
- Keep higher renderer tiers optional and capability-gated.

## Working Rules

- Read the relevant files in `specs/` before changing code or contracts.
- Keep shared behavior out of `products/` until it is genuinely reused.
- Prefer precise, local edits over broad rewrites.
- Verify the changed area before finalizing and update docs that became stale.

## Platform Versus Product

- Put code in `platform/` only when it is a stable shared concern or a public platform contract.
- Keep product identity, presets, content, and product-specific behavior inside the owning product subtree.

## Do Not

- Do not raise the language or platform baseline without updating the specs.
- Do not add mandatory third-party runtime dependencies.
- Do not introduce fake validation evidence, fake assets, or fake release claims.
- Do not let a product requirement silently redefine the platform contract.

Later series prompts are authoritative for their local scope, but they must preserve the normative contracts in `specs/` unless they update those contracts explicitly.
