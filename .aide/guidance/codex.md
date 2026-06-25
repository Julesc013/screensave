# Codex Guidance From AIDE Pilot

Current active control plane:

- Read `PROJECT_STATE.toml` for released/development/queue truth.
- Read `VERSION.toml` for runtime, ABI, schema, and proof-version counters.
- Read `catalog/products.toml` and `catalog/artifact_profiles.toml` before changing build graphs, package manifests, product naming, or compatibility claims.
- Treat `.codex/queues/` as historical-superseded lineage unless a future state file reopens queue authority.
- Keep AIDE report-only during this pilot.
- Do not add `.aide` references under `platform/` or `products/`.
- Do not widen compatibility claims without recorded evidence.
