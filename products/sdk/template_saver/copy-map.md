# Template Saver Copy Map

Copy `products/savers/_template/` to `products/savers/<slug>/`, then replace the following in one reviewable pass:

## Rename Tokens

- directory name `_template` -> `<slug>`
- file prefix `template_saver_` -> `<slug>_`
- manifest `[product] key=template_saver` -> `<slug>`
- manifest `[product] name=Template Saver` -> product display name
- registry root `Software\\Julesc013\\ScreenSave\\Products\\template_saver` -> canonical product registry root
- default preset and theme keys -> product-owned defaults

## Keep Shared

- the shared saver/module contract in `platform/include/screensave/`
- the shared settings and pack contract
- the Win32 saver entry pattern using `screensave_scr_main(...)`
- the renderer ladder and fallback expectations

## Product-Local Work You Must Replace

- preset descriptors and preset values
- theme descriptors and colors
- product config enums and meaning
- render and simulation behavior
- config dialog labels and IDs
- manifest summary and capability flags

## Build Integration Later

When the new saver is real enough to wire into the checked-in build lanes:

- add the saver project files or make targets beside the current saver line
- add the saver to the appropriate diagnostics and suite-app surfaces
- update validation notes and truth-bearing docs in the same change

Do not add a new saver to the build graph before its manifest, presets, themes, tests, and role docs are coherent.
