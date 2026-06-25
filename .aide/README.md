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
Local transient state belongs in `.aide.local/`, which is ignored.

Type: development control plane.
