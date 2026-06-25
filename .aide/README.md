# AIDE Control Plane

Purpose: report-only development governance for ScreenSave.

The `.aide/` tree may record:

- pilot scope and safety rules
- work units
- evidence packet references
- generated or hand-authored guidance for coding tools
- drift reports and proof summaries

It must not become:

- a runtime dependency
- a saver binary dependency
- an automatic merge authority
- a replacement for `PROJECT_STATE.toml`, `VERSION.toml`, `specs/`, or validators

The current pilot is [`pilot.toml`](pilot.toml).
The current report-only AIDE Lite pin is [`aide_lite.lock.toml`](aide_lite.lock.toml).
The current deterministic project bridge profile is
[`project_bridge_profile.toml`](project_bridge_profile.toml).
Local transient state belongs in `.aide.local/`, which is ignored.

Type: development control plane.
