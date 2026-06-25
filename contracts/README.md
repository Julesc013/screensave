# Contracts

Purpose: versioned ScreenSave contracts for portability, artifact proof, and
future generated surfaces.

The root `specs/` directory remains normative repository law. Files in this
directory are versioned contract records that specs, validators, tools, and
future generated outputs may adopt deliberately.

Current contracts:

- [`portable_semantics_v2.md`](portable_semantics_v2.md): planned host-neutral saver seam.
- [`renderer_aliases_v1.md`](renderer_aliases_v1.md): compatibility mapping for existing renderer names.
- [`screensave_doctrine_v1.md`](screensave_doctrine_v1.md): active ScreenSave product doctrine and AIDE boundary.
- [`proof_bundle_v0.md`](proof_bundle_v0.md): initial proof-bundle shape for future Lab Runner, Workbench, CI, and AIDE evidence packets.
- [`surface_rgba8_v0.md`](surface_rgba8_v0.md): private proof-kernel pixel-surface contract.
- [`proof_kernel_v0.md`](proof_kernel_v0.md): active ScreenSave-owned proof-kernel milestone.
- [`project_adapter_v0.md`](project_adapter_v0.md): ScreenSave-owned status, capability, validation, and proof command surface for external coordinators.

Contract rules:

- Do not treat a draft contract as implemented runtime behavior.
- Do not let a contract silently override `specs/`.
- Do not make saver artifacts depend on contract tooling at runtime.
- Promote implementation only through reviewed specs, validators, and evidence.

Type: versioned contract records.
