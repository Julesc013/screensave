# C16 Companion Channel Matrix

This note records the companion-channel artifact state frozen beside `ScreenSave Core` in `C16`.

| Channel | Artifact Surface | C16 State | Notes |
| --- | --- | --- | --- |
| `Installer` | `out/installer/screensave-installer-c16-baseline/` and `out/installer/screensave-installer-c16-baseline.zip` | refreshed staged package | Current-user installer companion built from the frozen Core payload |
| `Suite` | `out/msvc/vs2022/Release/suite/suite.exe` | separate companion app artifact | Real release-build app output; not bundled into Core or Installer |
| `BenchLab` | `out/msvc/vs2022/Release/benchlab/benchlab.exe` | separate diagnostics artifact | Real release-build diagnostics harness; not bundled into Core or Installer |
| `SDK` | `products/sdk/` | source-only companion surface | Contributor docs, examples, and validators remain source-controlled rather than packaged |
| `Extras` | none frozen in `C16` | empty companion channel | Reserved for lower-confidence or experimental material; no payload shipped in this freeze |

## Boundary Reminder

- Core remains the primary end-user saver ZIP.
- Companion channels remain separate by doctrine even when their binaries or sources exist beside the Core build.
- Post-`C16` work still runs `SS` first and `PL` second; companion channels do not become the main line during that handoff.
