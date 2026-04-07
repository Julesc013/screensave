# SX Series

This document defines the active `SX` program that sits after the closed `SS` bridge and before `PL`.

`SX` exists to harden the private renderer substrate and capability-expansion model without reopening the public product doctrine that was already frozen through `C16` and normalized through `SS`.

## Current Status

- `SS00`, `SS01`, and `SS02` are complete.
- Optional `SS03` remains a reserved maintenance-policy sidecar, not the active platform program.
- `SX00` and `SX01` are complete.
- `SX02` is now the active post-`SS` and pre-`PL` substrate step.
- `PL` begins only after `SX` hands off a proven substrate.

## What SX Is

- A short, disciplined substrate-hardening and capability-expansion series
- A post-release platform tranche that keeps the public renderer ladder intact
- A bounded bridge from the current `gdi` / `gl11` / `gl21` reality to a cleaner long-lived private substrate
- The place where stable internal capability bands, backend routing, and future service seams are defined before deeper flagship product work resumes

## What SX Is Not

- Not a product-line reset
- Not a release-doctrine reset
- Not a naming reset
- Not a saver-identity reset
- Not a suite-wide migration campaign
- Not a D3D, D2D, or DWrite adoption program
- Not a justification to replace the current public renderer ladder
- Not a substitute for `PL`
- Not permission to turn ScreenSave into a broad multi-API middleware empire

## Sequence

The current intended `SX` order is:

| Step | Role | Boundary |
| --- | --- | --- |
| `SX00` | substrate constitution and boundary freeze | define what remains public, what becomes private, and what later `SX` work may and may not assume |
| `SX01` | private substrate scaffolding and routing cleanup | establish the first real private band, backend, capability, routing, and present-path structure without changing the public ladder |
| `SX02` | current-tier hardening and proof-surface work | harden `gdi`, `gl11`, and `gl21` behavior plus substrate-facing proof surfaces without widening product scope |
| `SX03` | later modern-tier bring-up if justified | make `gl33` real only if the lower tiers remain honest and stable |
| `SX04` | later premium-tier bring-up if justified | make `gl46` real only after the earlier tiers and routing model are proven |
| optional `SX05` | substrate handoff checkpoint | record that the substrate is stable enough to support `PL` without reopening its constitution |

These steps are ordered on purpose.
Later higher-capability work must not skip substrate hardening of the lower tiers.

## Constitutional Constraints

- `ScreenSave Core` remains the primary product.
- Standalone `.scr` savers remain the main line.
- `anthology` remains a real saver product, not the `suite` app.
- `suite` remains the browser, launcher, preview, and configuration companion.
- BenchLab remains diagnostics-only.
- SDK remains contributor-facing only.
- Extras remains the experimental or lower-confidence holdback channel.
- The public renderer ladder remains `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, `null`.
- `gdi` remains the guaranteed floor.
- `gl11` remains the conservative accelerated tier.
- `gl21` remains the first real advanced tier.
- `gl33` and `gl46` remain future-capable tiers until later `SX` work makes them real.
- `null` remains internal only.
- The classic Win32 `.scr` host model remains intact.
- The public platform surface remains narrower and more stable than the private substrate.

## Public Versus Private Direction

During `SX`:

- The public contract stays anchored to the current renderer ladder and the current narrow saver and renderer APIs.
- Private substrate work may add backend kinds, capability bands, capability descriptors, backend registry or loader logic, present-path internals, and future service seams.
- Products should eventually target stable internal capability bands rather than backend-private details.
- None of those private additions become public merely because they exist.

The detailed rules live in:

- [../../specs/render_bands.md](../../specs/render_bands.md)
- [../../specs/backend_policy.md](../../specs/backend_policy.md)
- [../../specs/renderer_contract.md](../../specs/renderer_contract.md)

## Proof Surfaces And Later Product Work

- `gallery` is the most likely substrate proof and showcase saver during `SX` because it already exists to exercise the renderer ladder and honest fallback behavior.
- BenchLab remains the diagnostics-only surface for renderer and routing inspection during `SX`.
- `plasma` is not an `SX00` deliverable and is not the lead product of `SX`.
- `plasma` remains the first flagship `PL` track after `SX` closes.

## Glossary

- `public contract`: the stable renderer ladder, saver contract, compatibility doctrine, and public headers that products and releases are allowed to depend on directly
- `private substrate`: backend implementations, routing logic, capability descriptors, registries, and service seams that remain internal under `platform/src/`
- `backend kind`: a private implementation identity, which may later map to one public tier or one internal band without becoming public doctrine on its own
- `capability band`: a stable internal execution band used to group behavior and routing policy without exposing backend internals publicly
- `service seam`: a bounded internal support layer, such as image, text, or presentation support, that remains optional and private until proven durable
- `fallback walk`: the ordered downgrade path from a higher requested capability to the highest compatible lower path that still preserves truthful behavior
