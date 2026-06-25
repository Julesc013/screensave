# Catalog

Purpose: machine-readable product and artifact-profile authority.

The catalog is the source of truth for:

- product keys, roles, and maturity signals
- saver source-unit expectations used by build validators
- artifact profiles used for compatibility and release evidence
- future generated build, packaging, documentation, Manager, Workbench, and Anthology registries

`products.toml` records product identity and source/catalog metadata.
`artifact_profiles.toml` records the artifact lanes that may receive compatibility evidence.

Generated files may be added later, but the checked-in catalog remains the reviewed authority.

Type: repository control data.
