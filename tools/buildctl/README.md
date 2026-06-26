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

For real runs, the controller resolves MSBuild from the fixed profile policy:
the saver profile prefers VS2017 for the `v141_xp` lane, while the companion
tool profile prefers VS2022. It checks explicit environment overrides,
standard Visual Studio install paths, Visual Studio `vswhere`, then `PATH`.
The selected tool still belongs to the fixed profile command plan and callers
still cannot provide arbitrary build arguments.

Type: repository proof tooling.
