# buildctl

Purpose: fixed ScreenSave build-profile controller.

`screensave_build.py` exposes named build profiles instead of accepting
arbitrary compiler or MSBuild arguments. Each profile owns:

- the fixed build lane;
- the exact build command;
- the artifact sets expected from that lane;
- generated artifact-set manifests;
- profile-aware PE audits;
- one build receipt.

Examples:

```powershell
python tools\buildctl\screensave_build.py build --profile windows-current-x86
python tools\buildctl\screensave_build.py build --profile windows-current-tools
python tools\buildctl\screensave_build.py build --profile windows-current-x86 --dry-run
```

`--dry-run` records the exact command plan without invoking MSBuild. It is for
validators and capability review only; it is not build evidence.

Type: repository proof tooling.
