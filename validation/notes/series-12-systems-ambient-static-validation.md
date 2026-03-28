# Series 12 Systems And Ambient Static Validation

Date: 2026-03-29
Environment: Windows host with Python 3 and `dotnet` SDK available, but without Visual C++ MSBuild targets and without `mingw32-make`.

## Static Validation

Commands:
- `python tools/scripts/check_repo_structure.py`
- `python tools/scripts/check_codex_config.py`
- `python tools/scripts/check_build_layout.py`

Result:
- All three checks passed.
- The checked-in VS2022 solution and project graph now include `signal_lab.vcxproj`, `mechanical_dreams.vcxproj`, and `ecosystems.vcxproj`.
- The checked-in MinGW i686 make lane now includes all ten current saver products plus BenchLab.

## Toolchain Availability

Command:
- `dotnet msbuild build/msvc/vs2022/ScreenSave.sln /p:Configuration=Debug /p:Platform=Win32 /m`

Result:
- The .NET-hosted MSBuild front-end was present, but the run failed before compilation because `$(VCTargetsPath)\Microsoft.Cpp.Default.props` was unavailable in this environment.
- `MSBuild.exe` was not present on `PATH`, and `mingw32-make` was not installed.

## Notes

- This note does not claim a successful native Win32 build or smoke execution for Series 12.
- It records that the repo structure, Codex control-plane checks, and checked-in build-layout wiring are consistent with the widened ten-saver state.
