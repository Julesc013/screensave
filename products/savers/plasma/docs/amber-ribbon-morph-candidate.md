# Amber Ribbon Morph Candidate

Status: experimental Plasma candidate.

`amber_ribbon_morph` is a product-local runtime theme and matching preset based
on a user-supplied visual reference: dark violet negative space, molten amber
fields, yellow crest highlights, soft ribbon structure, and slow carried color
morphing.

Implemented now:

- built-in experimental preset key: `amber_ribbon_morph`
- built-in experimental theme key: `amber_ribbon_morph`
- warm theme morph class for bounded theme transitions
- plasma preset morph class and warm bridge class for the existing transition
  grammar
- v2 data-only candidate pack: `amber_ribbon_morph_v2`

Runtime boundary:

- stable defaults remain `plasma_lava`
- the candidate is not in stable sets
- lower lanes keep the GDI and GL11 floor; premium ribbon presentation clamps
  to flat when the active renderer cannot support it
- the carried color morphing uses the existing `PLASMA_EFFECT_PLASMA`
  procedural palette-cycle path, not a new renderer feature

Plasma v2 pack boundary:

- the v2 pack approximates the reference with `plasma_lava` material, high
  contrast, high brightness, radial warp, continuous output, and flat
  presentation
- Plasma v2 pack schema does not yet expose arbitrary amber/violet palette
  stops or ribbon presentation
- the pack is candidate review input only, not artistic acceptance, stable
  promotion, release evidence, or compatibility certification

BenchLab report-mode trial shape:

```powershell
.\out\msvc\vs2022\Debug\benchlab\benchlab.exe /deterministic /seed:744 /saver:plasma /frames:320 /renderer:auto /plasma-preset:amber_ribbon_morph /plasma-theme:amber_ribbon_morph /report:validation\captures\plasma-v2\visual-reference\benchlab-amber-ribbon-morph.txt
```

For the normal `.scr` host path, select the preset/theme through Plasma's
settings or persisted product config rather than treating BenchLab forcing as a
public saver command-line contract. Keep any screenshot or hardware judgement
as separate review evidence.
