# Platform Source

Purpose: private implementation for the host, core runtime, renderer backends, and the `SX01` substrate split that now sits beneath the stable public contract.
Belongs here: platform internals that support the shared runtime.
Does not belong here: product-local code or release artifacts.
Type: runtime implementation.

Key private entry points:

- `core/base/` for public-contract dispatch glue and saver-contract helpers
- `core/substrate/` for backend selection, capability capture, present-path routing, and future private service seams
- `render/` for concrete backend implementations behind the substrate layer
- `host/` for the Win32 `.scr` host path
