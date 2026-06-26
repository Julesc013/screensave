# ScreenSave Project Adapter

Purpose: expose a small ScreenSave-owned protocol surface for AIDE, CI, worker
harnesses, and local scripts.

Commands:

```powershell
python tools\project_adapter\screensave_project.py status
python tools\project_adapter\screensave_project.py capabilities
python tools\project_adapter\screensave_project.py catalog
python tools\project_adapter\screensave_project.py profiles
python tools\project_adapter\screensave_project.py validate --tier T0
python tools\project_adapter\screensave_project.py validate --tier T1
python tools\project_adapter\screensave_project.py validate --tier T2
python tools\project_adapter\screensave_project.py build --profile windows-current-x86 --invocation-id local-build
python tools\project_adapter\screensave_project.py render --invocation-id local-render
python tools\project_adapter\screensave_project.py compare --actual validation\captures\proof-kernel-v0\nocturne\capture.ppm
python tools\project_adapter\screensave_project.py audit --artifact-profile windows_current_x86_scr
python tools\project_adapter\screensave_project.py proof --profile nocturne.reference.v0 --invocation-id local-proof
python tools\project_adapter\screensave_project.py proof --profile ricochet.reference.v1 --invocation-id local-proof
python tools\project_adapter\screensave_project.py bundle --profile nocturne.reference.v0 --invocation-id local-bundle
python tools\project_adapter\screensave_project.py bundle --profile ricochet.reference.v1 --invocation-id local-bundle
```

The adapter reports JSON receipts. It delegates to ScreenSave validators,
fixed build profiles, and `sslab`; it does not edit code, merge branches,
certify compatibility, publish releases, or make AIDE part of saver runtime.

The `validate` command accepts fixed tiers only. `T0` covers authority, syntax,
docs, AIDE pilot, and whitespace checks. `T1` includes `T0` and adds affected
catalog, adapter, `libsslab`, proof, and Workbench checks. `T2` includes `T0`
and `T1` and adds the portable v2 header seam check, the full local project
gate, plus fixed Nocturne and Ricochet profile proofs. T3 remains an
operator-scheduled native evidence gate, not an admitted AIDE fixed capability.

The `build` command accepts named profiles only. Current profiles are
`windows-current-x86` and `windows-current-tools`; callers cannot provide
arbitrary MSBuild, compiler, linker, or output arguments. Dry-run receipts prove
the admitted command plan but are not build evidence.

The `profiles` command reports the fixed admitted proof profile keys. The
`proof` and `bundle` commands accept only `nocturne.reference.v0` and
`ricochet.reference.v1`. They do not expose `screensave.proof.any-profile` or
arbitrary command execution.

Generated-output commands use contained invocation roots under
`out/aide/screensave-project-adapter/invocations/`. Callers provide a sanitized
`--invocation-id`; they do not provide arbitrary output paths. Compare inputs
are limited to ScreenSave capture/proof roots. PE audit inputs are selected by
artifact profile and resolved through `artifact_profile_audit_roots.json`; the
adapter does not accept arbitrary audit paths.

The fixed capability profile is `capability_bindings.json`.
Command-specific receipt requirements are recorded in `receipt_schemas.json`.

Type: repository control tool.
