# C16 Known Issues And Limits

This note records the explicit non-blocking limitations that still remain after the `SS02` evidence refresh for the frozen `C16` baseline.

## Current Known Issues

- Fresh `SS02` evidence now exists for release-asset integrity, the full nineteen-saver `gdi` fullscreen line, representative `Settings...` checks, representative `gl11` and `gl21` probes, and installer mock-mode current-user install and uninstall.
- Fresh Screen Saver Settings discovery and `desk.cpl` listing evidence is still not recorded in `SS02`.
- Preview `/p` was attempted on `nocturne` and `anthology`, but child-window parenting was not confirmed; preview remains unverified rather than pass/fail.
- `gl11` and `gl21` remain optional capability tiers. `SS02` refreshed representative renderer evidence, but it did not rerun a full accelerated-renderer matrix across every saver.
- `gl33` and `gl46` remain future placeholders rather than shipped release tiers. Same-build BenchLab requests for those tiers fell back to `gl21`.
- The companion Installer channel remains current-user only. `SS02` verified mock-mode current-user install and uninstall, but it did not claim fresh live registry or Control Panel registration smoke.
- Machine-wide install remains unsupported.
- `suite` and BenchLab remain separate companion artifacts and are not wrapped into the end-user Core or Installer downloads.
- Extras remains an intentionally empty channel in `C16`; no experimental payload was frozen.
- The broader Windows preservation band from `specs/compatibility.md` remains the target, but `SS02` only reran Windows 10 x64 locally.

## Release Impact

- These limits do not block the frozen Core baseline because the published Core ZIP now has direct `SS02` evidence for release integrity, the full `gdi` saver line, representative settings behavior, and bounded installer behavior.
- These limits do block any claim that the frozen release has fresh end-to-end Control Panel, preview-child-hosting, legacy-OS, machine-wide install, or full accelerated-renderer coverage everywhere.
- These limits now feed either optional `SS03` maintenance policy work or later bounded maintenance fixes; this note does not reopen the closed product doctrine.
