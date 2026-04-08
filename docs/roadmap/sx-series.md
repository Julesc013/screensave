# SX Series

This document defines the active `SX` program that sits after the closed `SS` bridge and before `PL`.

`SX` exists to harden the private renderer substrate and capability-expansion model without reopening the public product doctrine that was already frozen through `C16` and normalized through `SS`.

## Current Status

- `SS00`, `SS01`, and `SS02` are complete.
- Optional `SS03` remains a reserved maintenance-policy sidecar, not the active platform program.
- `SX00`, `SX01`, `SX02`, `SX03`, `SX04`, `SX05`, `SX06`, `SX07`, `SX08`, and `SX09` are complete.
- `SX09` closed the series with a bounded proof matrix, deterministic BenchLab capture support, and an explicit `PL00` substrate handoff.
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
| `SX02` | GDI floor hardening plus current-tier proof-surface cleanup | harden the universal `gdi` floor first, tighten fallback landing and diagnostics, and keep current-tier proof surfaces honest without widening product scope |
| `SX03` | GL11 parity lane | harden the conservative accelerated compat lane, tighten GL11 setup, teardown, present, and fallback behavior, and keep it parity-first over the GDI floor |
| `SX04` | GL21 advanced-lane hardening | harden the first real advanced tier around an explicit private capability bundle and disciplined degradation to `gl11` and `gdi` |
| `SX05` | GL33 modern-lane bring-up | make `gl33` real as the first bounded modern tier without weakening lower-tier discipline |
| `SX06` | GL46 premium-lane bring-up | make `gl46` real as the bounded premium tier without weakening the lower-tier degradation model |
| `SX07` | future-service seams | add private image, text, present-policy, and future-backend-family seams without turning them into public doctrine or adopted non-GL families |
| `SX08` | capability manifests and routing policy | make saver intent, representative preset and pack metadata, and central routing outcomes explicit and diagnosable without rewriting the public renderer doctrine |
| `SX09` | substrate handoff checkpoint | prove the finished ladder, strengthen BenchLab routing observability and deterministic reports, and hand `PL00` a bounded evidence-backed substrate contract |

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
- `gl33` is now the first real modern tier after `SX05`.
- `gl46` is now the real premium tier after `SX06`.
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

- `gallery` is the bounded substrate proof and showcase saver during `SX`; `SX05` uses it as the first honest `gl33` proof surface and `SX06` extends that same bounded surface to the real optional `gl46` premium lane without turning either tier into a platform-wide migration requirement.
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
