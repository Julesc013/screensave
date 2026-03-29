# MSVC Build Lanes

This directory groups Microsoft Visual C++ lanes by generation.

## Why Multiple Generations Exist

ScreenSave targets a wide Windows band.
That means the repository must preserve space for:

- a modern practical lane that contributors can extend now
- older MSVC generations that matter to long-term compatibility validation

The checked-in layout makes those lanes explicit instead of hiding them behind one modern-only abstraction.

## Current State

Real now:

- `vs2022/` contains the concrete modern solution and project files used to build the shared core runtime, the real host skeleton, the mandatory GDI backend, the optional GL11 and GL21 capability paths, the current canonical saver line as one true `.scr` output per saver through `anthology.scr`, and the real `benchlab.exe` diagnostics app target.

Documented scaffold only:

- `vs6/`
- `vs2008/`

Those older lanes are intentionally documentation-heavy at the current `C03` baseline.
They preserve file layout and intent without claiming build completeness that does not yet exist.

## Output Intent

All MSVC lanes use the shared `out/` conventions from `build/README.md`.
Toolchain-specific project files live here; generated outputs do not.
