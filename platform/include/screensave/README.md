# Public Headers

Purpose: public headers for shared platform contracts.
Belongs here: stable cross-product types, interfaces, and constants once they are proven.
Does not belong here: private helpers or product-specific declarations.
Type: public runtime contract.

Current stage: the first narrow public header set remains real here, with Series 06 now using it for the first real saver product:

- `types.h` for tiny shared geometry and color structures
- `version.h` for runtime version identity
- `diagnostics_api.h` for lightweight diagnostics reporting
- `config_api.h` for shared common-config, preset, and theme descriptors
- `renderer_api.h` for the renderer contract now exercised by the mandatory GDI backend
- `saver_api.h` for the saver/module lifecycle boundary, shared host entry, and product-owned config hooks

Implementation details remain private under `platform/src/`.
