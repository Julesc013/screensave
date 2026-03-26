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
Type: runtime implementation.
