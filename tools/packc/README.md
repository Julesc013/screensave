# packc

`packc` validates and compiles bounded, data-only ScreenSave packs.

The PAW-DX slice admits only `screensave.plasma.v2` packs. Output is written
under `out/` and contains canonical JSON plus hash manifests. Packs are data
only: scripts, DLLs, macros, absolute paths, and parent traversal are rejected.
VisualIntent Plasma examples are admitted only as explicit `plasma_spec_v2`
data with the `plasma.v2.visualintent.preview` proof-profile reference.

Examples:

```powershell
py -3 tools\packc\packc.py validate products\savers\plasma\content\v2\examples\plasma_lava_v2.toml
py -3 tools\packc\packc.py compile products\savers\plasma\content\v2\examples\plasma_lava_v2.toml --out out\packc\plasma_lava_v2
py -3 tools\packc\packc.py validate products\savers\plasma\content\v2\visualintent\calm_dark_primary.toml
```
