# ScreenSave Project Adapter

Purpose: expose a small ScreenSave-owned protocol surface for AIDE, CI, worker
harnesses, and local scripts.

Commands:

```powershell
python tools\project_adapter\screensave_project.py status
python tools\project_adapter\screensave_project.py capabilities
python tools\project_adapter\screensave_project.py validate
python tools\project_adapter\screensave_project.py proof --output-dir out\proof\project-adapter\nocturne
```

The adapter reports JSON receipts. It delegates to ScreenSave validators and
`sslab`; it does not edit code, merge branches, certify compatibility, or make
AIDE part of saver runtime.

Type: repository control tool.
