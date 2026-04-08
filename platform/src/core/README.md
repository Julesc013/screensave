# Core Runtime

Purpose: shared non-renderer runtime services used by the platform.
Belongs here: timing, config resolution, state helpers, and other cross-product internals.
Does not belong here: renderer-specific code or product-local behavior.
Current stage: Series 04 makes the first real core-runtime layer live here:

- `base/` for renderer dispatch glue and saver-contract helpers
- `substrate/` for private renderer-substrate routing, backend descriptors, capability capture, present-path policy, and future optional service seams introduced in `SX01` and expanded in `SX07`
- `timing/` for shared frame-time sampling helpers
- `rng/` for deterministic portable RNG helpers
- `diagnostics/` for lightweight shared diagnostics state and emission
- `config/` for common config, preset, and theme helpers
- `version/` for runtime version identity

Renderer backends now consume these contracts from `platform/src/render/`.
Series 06 uses the core runtime from the first real saver product without promoting Nocturne-specific behavior into shared code.
Series 07 adds only narrow shared helpers here for saver-config state ownership and diagnostics usage shared by the `.scr` host and BenchLab.
Series 08 adds only narrow renderer-selection and fallback-reporting glue here so GDI remains the guaranteed floor while GL11 stays optional.
Series 09 adds only a narrow private saver-registry helper here so the host and BenchLab can resolve the current built-in saver set without introducing broad discovery or plugin machinery.
Series 10 adds only a narrow shared software-visual-buffer helper here so Plasma and Phosphor can share bitmap- and persistence-oriented drawing without promoting a broad effects or vector framework.
Series 11 adds only a narrow shared byte-grid helper here so Pipeworks and Lifeforms can share small occupancy and double-buffered state storage without promoting a broad simulation or tile framework.
Series 15 adds only narrow tiered renderer-selection and fallback-reporting glue here so GDI remains the guaranteed floor, GL11 remains the conservative accelerated middle tier, GL21 stays strictly optional and capability-gated, GL33 and GL46 can stay explicit in the public ladder, and the null backend remains an internal emergency fallback only.
SX01 keeps the public renderer contract intact while moving backend kinds, band routing, capability capture, present-path policy, and future image/text service seams into the new private `substrate/` layer.
SX05 and SX06 then make the `modern` and `premium` bands real without widening the public renderer contract, so this core layer continues to own selection, fallback, and private caps reporting while backend-private lifecycle stays in `platform/src/render/`.
SX07 keeps those lanes primary while expanding the same private core layer with typed image, text, present-policy, and backend-family seams plus a small private registry that can host later optional adapters without dragging products onto private headers.
Type: runtime implementation.
