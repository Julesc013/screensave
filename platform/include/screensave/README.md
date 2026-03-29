# Public Headers

Purpose: public headers for shared platform contracts.
Belongs here: stable cross-product types, interfaces, and constants once they are proven.
Does not belong here: private helpers or product-specific declarations.
Type: public runtime contract.

Current stage: the first narrow public header set remains real here, with Series 11 now using it for multiple real saver products plus the first real diagnostics app:

- `types.h` for tiny shared geometry and color structures
- `version.h` for runtime version identity
- `diagnostics_api.h` for lightweight diagnostics reporting
- `config_api.h` for shared common-config, preset, and theme descriptors
- `settings_api.h` for versioned settings documents, pack manifests, and shared import/export validation helpers
- `renderer_api.h` for the renderer contract now exercised by the mandatory GDI backend, the optional GL11 backend, and BenchLab's renderer-selection harness path
- `saver_api.h` for the saver/module lifecycle boundary, shared host entry, product-owned config hooks, and reusable saver-config state helpers
- `grid_buffer_api.h` for the narrow shared byte-grid helper promoted in Series 11 for Pipeworks and Lifeforms
- `visual_buffer_api.h` for the narrow shared software-framebuffer helper promoted in Series 10 for Plasma and Phosphor

Implementation details remain private under `platform/src/`.
