# PAW-01 Local Windows Current Build Evidence

Date: 2026-06-25.

Status: local development evidence, not release-grade certification.

## Commands

```powershell
python tools\buildctl\screensave_build.py build --profile windows-current-x86 --output-dir out\buildctl\windows-current-x86-local
python tools\buildctl\screensave_build.py build --profile windows-current-tools --output-dir out\buildctl\windows-current-tools-local
```

## Result

- `windows-current-x86`: pass.
- Expected saver artifacts: 19.
- Observed expected saver artifacts: 19.
- Unexpected saver artifacts: 0.
- PE profile: `windows_current_x86_scr`.
- PE audit status: informational.
- PE artifact count: 19.
- PE violation count: 0.

- `windows-current-tools`: pass.
- Expected tool artifacts: 2.
- Observed expected tool artifacts: 2.
- Unexpected tool artifacts: 0.
- PE profile: `windows_current_tool`.
- PE audit status: informational.
- PE artifact count: 2.
- PE violation count: 0.

## Important Limits

- The run occurred while the repository had uncommitted build-controller changes,
  so it is not clean-source release evidence.
- The PE audits record binary facts only and do not certify operating-system
  compatibility.
- The generated binaries are current Windows artifacts. They are not
  preservation-profile evidence.
- The audit notes dynamic VC/UCRT dependencies and subsystem version 6.0.
- The first saver build reported MSVC warning C4101 for unused local variables
  `band_y` and `next_band_y` in `products/savers/explorer/src/explorer_render.c`.

The durable proof outputs are under ignored `out/buildctl/` paths on the local
machine. Future release evidence should be regenerated from a clean checkout or
captured from the hosted Windows CI artifact.
