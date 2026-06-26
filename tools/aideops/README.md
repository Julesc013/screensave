# ScreenSave AIDE Ops

`screensave_aide.py` is a thin ScreenSave-local operator for the imported AIDE
Lite command surface. It runs AIDE Lite commands in a fixed sequence and writes
operation receipts under `out/aide/operations/`.

It is deliberately not a context compiler, evaluator, evidence model, worker,
source editor, merge helper, or release promoter. Provider calls, model calls,
network calls, source mutation, branch mutation, and automatic promotion remain
false for every receipt.

Common commands:

```powershell
py -3 tools\aideops\screensave_aide.py status
py -3 tools\aideops\screensave_aide.py bootstrap
py -3 tools\aideops\screensave_aide.py preflight --task SS-PV2-00 --objective "Define the portable v2 base contract"
py -3 tools\aideops\screensave_aide.py postflight --task SS-PV2-00
py -3 tools\aideops\screensave_aide.py full-check
```
