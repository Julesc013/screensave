# C16 Known Issues And Limits

This note records the explicit non-blocking limitations left in the frozen `C16` baseline.

## Current Known Issues

- `C16` produced a fresh `Release|Win32` build, but it did not rerun live Control Panel or `desk.cpl` screen saver smoke.
- `gl11` and `gl21` remain optional capability tiers and were not re-smoke-tested on every saver during `C16`.
- The companion Installer channel remains current-user only; machine-wide install and richer repair modes remain deferred.
- `suite` and BenchLab are separate companion artifacts and are not wrapped into the end-user Core or Installer downloads.
- Extras remains an intentionally empty channel in `C16`; no experimental payload was frozen.

## Release Impact

- These limits do not block the frozen Core baseline because the full canonical saver line built successfully as x86 Win32 release artifacts and the channel boundaries are explicit.
- These limits do block any claim that `C16` completed fresh end-to-end runtime validation on every supported Windows revision.
- These limits feed later `SS02` evidence refresh work; this note does not claim that refresh is already complete.
