# Plasma PL06 Content-System Proof

## Purpose

This note records the actual proof work run for `PL06`.
It is limited to what was really rebuilt and executed for the new Plasma content registry and selection layer.

## Commands Used

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug CC=gcc "../../../out/mingw/i686/debug/plasma/plasma.scr"

gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
  products/savers/plasma/tests/smoke.c `
  out/intermediate/mingw/i686/debug/plasma/plasma_config.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_content.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_module.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_output.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_plan.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presentation.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presets.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_render.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_selection.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_sim.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_themes.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_treatment.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_validate.o `
  out/intermediate/mingw/i686/debug/screensave_platform/libscreensave_platform.a `
  -ladvapi32 -lgdi32 -luser32 -lopengl32 `
  -o out/mingw/i686/debug/plasma/plasma_smoke.exe

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"; $LASTEXITCODE
```

## What Was Validated

- The new PL06 content registry validates successfully.
- The registry still represents the preserved built-in Plasma Classic preset and theme inventory.
- The shipped `lava_remix` pack remains represented honestly and its manifest still loads from the repo.
- The default content-selection posture stays on:
  - preset `plasma_lava`
  - theme `plasma_lava`
  - `stable_only`
  - no active preset or theme set
- Alias normalization still works for content selection metadata, including `ember_lava -> plasma_lava`.
- Product-local preset/theme sets resolve content deterministically without changing the default baseline.
- Favorites and exclusions resolve content honestly and do not bypass exclusions.
- Experimental-only filtering clamps back to the stable pool because the current repo does not yet ship experimental Plasma content.
- Product-local content metadata imports and exports through Plasma's preset settings hooks.
- Product-specific randomization still leaves the persisted content-selection layer untouched.
- The preserved classic/default session path still allocates and runs through the shared saver contract.

## Remaining Limits

- This proof note does not claim any new public UI for favorites, exclusions, or sets.
- This proof note does not claim dynamic user or portable content discovery.
- This proof note does not claim morph or journey runtime behavior.

## Evidence Boundary

Actually run:

- rebuilt `plasma.scr`
- rebuilt `plasma_smoke.exe`
- executed `plasma_smoke.exe` successfully with exit code `0`

Documented but not claimed as run here:

- interactive config-dialog editing of the new product-local selection state
- live BenchLab inspection of the content registry
- later transition or journey execution against the content layer
