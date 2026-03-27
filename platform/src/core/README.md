# Core Runtime

Purpose: shared non-renderer runtime services used by the platform.
Belongs here: timing, config resolution, state helpers, and other cross-product internals.
Does not belong here: renderer-specific code or product-local behavior.
Current stage: Series 04 makes the first real core-runtime layer live here:

- `base/` for renderer dispatch glue and saver-contract helpers
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
Type: runtime implementation.
