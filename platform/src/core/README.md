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

Renderer backends remain deferred to later series.
Type: runtime implementation.
