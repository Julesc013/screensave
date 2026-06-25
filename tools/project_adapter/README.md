# ScreenSave Project Adapter

Purpose: expose a small ScreenSave-owned protocol surface for AIDE, CI, worker
harnesses, and local scripts.

Commands:

```powershell
python tools\project_adapter\screensave_project.py status
python tools\project_adapter\screensave_project.py capabilities
python tools\project_adapter\screensave_project.py catalog
python tools\project_adapter\screensave_project.py validate
python tools\project_adapter\screensave_project.py render --invocation-id local-render
python tools\project_adapter\screensave_project.py compare --actual validation\captures\proof-kernel-v0\nocturne\capture.ppm
python tools\project_adapter\screensave_project.py audit --artifact-profile windows_current_x86_scr
python tools\project_adapter\screensave_project.py proof --invocation-id local-proof
```

The adapter reports JSON receipts. It delegates to ScreenSave validators and
`sslab`; it does not edit code, merge branches, certify compatibility, or make
AIDE part of saver runtime.

The `proof` command is a combined Nocturne v0 receipt: deterministic render,
exact capture comparison, artifact-profile PE audit facts, and an artifact
manifest. The PE audit facts are binary evidence only; they are not operating
system compatibility certification.

Generated-output commands use contained invocation roots under
`out/aide/screensave-project-adapter/invocations/`. Callers provide a sanitized
`--invocation-id`; they do not provide arbitrary output paths. Compare inputs
are limited to ScreenSave capture/proof roots. PE audit inputs are selected by
artifact profile and resolved through `artifact_profile_audit_roots.json`; the
adapter does not accept arbitrary audit paths.

The fixed capability profile is `capability_bindings.json`.
Command-specific receipt requirements are recorded in `receipt_schemas.json`.

Type: repository control tool.
