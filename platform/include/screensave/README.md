# Public Headers

Purpose: public headers for shared platform contracts.
Belongs here: stable cross-product types, interfaces, and constants once they are proven.
Does not belong here: private helpers or product-specific declarations.
Type: public runtime contract.

Current stage: Series 04 makes the first narrow public header set real:

- `types.h` for tiny shared geometry and color structures
- `version.h` for runtime version identity
- `diagnostics_api.h` for lightweight diagnostics reporting
- `config_api.h` for shared common-config, preset, and theme descriptors
- `renderer_api.h` for the renderer contract without a backend implementation yet
- `saver_api.h` for the saver/module lifecycle boundary and shared host entry

Implementation details remain private under `platform/src/`.
