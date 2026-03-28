# Series 10 Framebuffer And Vector Build And Smoke

Date: 2026-03-28
Environment: Windows host with Visual Studio 2022 Enterprise MSBuild and Win32 debug tooling.

## Build

Command:
`D:\Programs\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe build/msvc/vs2022/ScreenSave.sln /p:Configuration=Debug /p:Platform=Win32 /m /nologo`

Result:
- Passed.
- Produced `screensave_platform.lib`, `nocturne.scr`, `ricochet.scr`, `deepfield.scr`, `ember.scr`, `oscilloscope_dreams.scr`, and `benchlab.exe` under `out/msvc/vs2022/Debug/`.

## Static Validation

Commands:
- `python tools/scripts/check_repo_structure.py`
- `python tools/scripts/check_codex_config.py`
- `python tools/scripts/check_build_layout.py`

Result:
- All three checks passed.

## Smoke

Smoke executables were compiled against the built Win32 debug objects and shared platform library for:
- `nocturne`
- `ricochet`
- `deepfield`
- `ember`
- `oscilloscope_dreams`
- `benchlab`

Result:
- All six smoke executables returned success.
- The saver smoke checks covered module validity, default config/clamp behavior, required preset/theme presence, and basic session allocation for the new framebuffer/vector products.
- The BenchLab smoke check covered default product selection plus registry lookup for `ricochet`, `deepfield`, `ember`, and `oscilloscope_dreams` alongside the default `nocturne` target.

## Notes

- The smoke-link step emitted the expected MSVC debug-link warning `LNK4075` while mixing ad hoc smoke executables with existing debug objects, but the executables linked and ran successfully.
- This note does not claim manual preview-window or long-run visual capture evidence.
