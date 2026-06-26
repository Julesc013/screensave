# Codex Guidance From AIDE Lite Control Plane

Current active control plane:

- Read `PROJECT_STATE.toml` for released/development/queue truth.
- Read `VERSION.toml` for runtime, ABI, schema, and proof-version counters.
- Read `catalog/products.toml` and `catalog/artifact_profiles.toml` before changing build graphs, package manifests, product naming, or compatibility claims.
- Treat `.codex/queues/` as historical-superseded lineage unless a future state file reopens queue authority.
- Use AIDE Lite operationally for governance, context, repo intelligence, and review, but keep source mutation and automatic merge disabled.
- Treat `.aide/aide_lite.lock.toml` as the admitted AIDE Lite source pin;
  do not vendor AIDE or make it required for ScreenSave builds.
- Treat `.aide/profile.yaml` as the target-local AIDE operating profile.
- Treat `docs/roadmap/aide-to-screensave-integration-plan.md` as the current
  fixed-command bridge admission plan.
- Use `.aide/project_bridge_profile.toml` and
  `tools/project_adapter/capability_bindings.json` for the deterministic
  ScreenSave bridge surface.
- Use `tools/buildctl/screensave_build.py` only through fixed build profiles;
  dry-run receipts are command-plan evidence, not native build evidence.
- Do not expose `screensave run <anything>`; use fixed adapter capabilities
  only.
- Do not treat fixed command receipts as worker-host acceptance, compatibility
  certification, release approval, or visual-artistic acceptance.
- Keep external-sidecar-only boundaries intact until later reviewed AIDE-side
  bridge and durable capability-invocation tasks are accepted.
- Do not add `.aide` references under `platform/` or `products/`.
- Do not widen compatibility claims without recorded evidence.
