# Series 09 Motion Family Build And Smoke

Date: 2026-03-28
Environment: Windows host with Visual Studio 2022 Enterprise MSBuild and Win32 debug tooling.

## Build

Command:
`MSBuild.exe build/msvc/vs2022/ScreenSave.sln /p:Configuration=Debug /p:Platform=Win32 /m /nologo`

Result:
- Passed.
- Produced `screensave_platform.lib`, `nocturne.scr`, `ricochet.scr`, `deepfield.scr`, and `benchlab.exe` under `out/msvc/vs2022/Debug/`.

## Smoke

Smoke executables were compiled against the built Win32 debug objects and shared platform library for:
- `nocturne`
- `ricochet`
- `deepfield`
- `benchlab`

Result:
- All four smoke executables returned success.
- The saver smoke checks covered module validity, config defaults and clamp behavior, and required preset/theme presence.
- The BenchLab smoke check covered default product selection plus registry lookup for `nocturne`, `ricochet`, and `deepfield`.

## Notes

- The smoke-link step emitted MSVC linker warnings about debug object defaults (`LNK4075` and `LNK4098`), but the executables linked and ran successfully.
- This note does not claim manual preview-window or long-run visual capture evidence.
