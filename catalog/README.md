# Catalog

Purpose: machine-readable product and artifact-profile authority.

The catalog is the source of truth for:

- product keys, roles, and maturity signals
- saver source-unit expectations used by build validators
- artifact profiles used for compatibility and release evidence
- exact artifact sets used by build and audit manifests
- proof profiles used by deterministic runner and Workbench planning
- future generated build, packaging, documentation, Manager, Workbench, and Anthology registries

`products.toml` records product identity and source/catalog metadata.
`artifact_profiles.toml` records the artifact lanes that may receive compatibility evidence.
`artifact_sets.toml` records expected product artifacts for named build lanes.
`proof_profiles.toml` records deterministic product proof inputs and baseline references.

`catalog/generated/` contains deterministic outputs produced by
`tools/cataloggen/cataloggen.py`. These generated files are committed so legacy
and preservation-oriented lanes can consume inventory data without requiring
Python at build time.

Type: repository control data.
