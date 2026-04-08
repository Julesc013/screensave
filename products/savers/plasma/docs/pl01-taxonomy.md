# Plasma PL01 Taxonomy Freeze

Status: product-local taxonomy and vocabulary freeze after `PL00`.

`PL01` freezes Plasma's canonical vocabulary and internal ontology.
It does not implement engine work, parser work, transitions, journeys, or new renderer behavior.

## Purpose

This document exists to:

- remove ambiguity from Plasma product terms
- freeze the taxonomy later `PL` phases must use
- separate current repo-supported terms from target flagship vocabulary
- give `PL02` and `PL03` a stable naming system without reopening `PL00`

## Relationship To PL00

[`pl00-constitution.md`](./pl00-constitution.md) froze Plasma's identity, boundaries, band truth, migration duties, and future commitments.
This document freezes the exact vocabulary that implements those commitments.

`PL01` inherits these settled `PL00` rules directly:

- Plasma is field-derived first.
- `contour` is an output-family concept, not a generator family.
- `ascii` and `matrix` are output modes under a glyph family.
- treatment stacking uses fixed slots only
- lower-band truth, Plasma Classic preservation, and experimental-first identity edges remain binding

## Current Repo-Supported Subset Vs Target Taxonomy

Current repo support is still much narrower than the target taxonomy frozen here.

| Area | Current repo support | PL01 freeze |
| --- | --- | --- |
| generator vocabulary | legacy `effect_mode` values `plasma`, `fire`, `interference` | canonical generator-family taxonomy |
| output vocabulary | implicit classic raster output only | canonical output-family and output-mode taxonomy |
| treatment vocabulary | legacy `smoothing` values `off`, `soft`, `glow` | fixed treatment-slot taxonomy |
| presentation vocabulary | implicit flat presentation | dimensional presentation taxonomy |
| theme vocabulary | concrete theme keys and display names only | theme-family taxonomy plus concrete-theme meaning |
| policy vocabulary | saver-level `quality_class`, manifest safety claims, some pack routing metadata | full profile, quality, safety, degrade, and transition taxonomy |
| transition vocabulary | none | frozen vocabulary only, no engine claim |

Important bridge rules:

- current classic `interference` content remains valid, but the canonical family name is `wave`
- current `effect_mode` values are legacy controls, not the full future generator taxonomy
- current `smoothing` is a legacy bridge control, not the final treatment-slot model
- current preset/theme keys and aliases remain valid migration inputs even when canonical taxonomy terms become more precise

## Ontology / Glossary

| Term | Frozen meaning |
| --- | --- |
| `generator_family` | the canonical family id for the source behavior that produces the primary field |
| `modifier` | a named transformation category applied after the generator and before final output interpretation |
| `output_family` | the primary way a field result is interpreted for display |
| `output_mode` | a named variant within one output family |
| `treatment` | an optional bounded presentation adjustment applied through one fixed slot |
| `treatment_slot` | the named pipeline location in which a treatment may appear |
| `presentation_mode` | the bounded flat, 2.5D, or 3D-style display posture applied after output and treatment work |
| `theme_family` | the palette or material lineage a concrete theme belongs to |
| `profile_class` | the stable or experimental publication posture of content |
| `quality_class` | the declared cost and uplift posture of content or routing intent |
| `safety_tag` | a product-meaningful operational safety label such as `preview_safe` |
| `preset` | a curated behavior bundle with stable identity |
| `theme` | a concrete appearance bundle that does not replace the generator identity |
| `pack` | a data-only bundle of product-owned content references |
| `preset_set` | a named curated collection of presets |
| `theme_set` | a named curated collection of themes |
| `transition` | an explicit rule for moving between two compatible content states |
| `transition_set` | a named reusable collection of transition rules |
| `journey` | an ordered or weighted travel plan across presets, themes, and transitions |
| `morph_compatibility_class` | the compatibility grade that determines whether direct morphing is honest |
| `degrade_policy` | the explicit ordered vocabulary for lower-band simplification or honest refusal |

## Generator Family Taxonomy

Status values in the table are literal:

- `implemented_today`
- `stable_target`
- `experimental_first`

| Family | Status | Frozen meaning | Current repo note |
| --- | --- | --- | --- |
| `plasma` | `implemented_today` | classic multi-source scalar-field plasma motion | current `effect_mode=plasma` |
| `fire_like` | `implemented_today` | upward or heat-biased abstract combustion-like field behavior without literal scenery | current `effect_mode=fire` |
| `wave` | `implemented_today` | layered interference, ripple, or phase-wave field behavior | current `effect_mode=interference`; `interference` remains a valid classic label |
| `water` | `stable_target` | abstract liquid-surface motion without becoming scenic water rendering | not a current field id |
| `cloud` | `stable_target` | soft volumetric-feel field behavior without scenic sky identity | not a current field id |
| `vapour` | `stable_target` | diffuse drifting field behavior lighter than cloud and less liquid than water | not a current field id |
| `wind` | `stable_target` | flow-dominant abstract field behavior emphasizing directional motion | not a current field id |
| `rain_like_abstract` | `stable_target` | abstract streak or droplet-pattern field behavior without literal weather scene framing | not a current field id |
| `lava` | `stable_target` | molten, heavy, field-derived motion family distinct from merely warm palette mapping | current `plasma_lava` naming is preset/theme identity, not generator metadata |
| `aurora` | `stable_target` | curtain-like luminous field family distinct from merely cool palette mapping | current `aurora_plasma` naming is preset identity, not generator metadata |
| `chemical_cellular_growth` | `experimental_first` | abstract, field-derived chemical, cellular, or growth-like behavior kept non-literal and non-ecological | not a current field id |
| `lattice_quasi_crystal` | `stable_target` | structured interference or tessellated field behavior that stays abstract and non-object-like | not a current field id |
| `caustic_marbling` | `stable_target` | refractive, marbled, or caustic-feel field behavior that stays abstract and surface-free | not a current field id |
| `substrate_vein_coral` | `experimental_first` | branching, veined, or coral-like abstractions that risk literal/ecological reading | not a current field id |
| `arc_discharge` | `stable_target` | abstract electrical or discharge-style field behavior without scenic machinery framing | not a current field id |

Rules:

- Generator families are source-behavior identities, not theme names.
- Current classic labels such as `interference` survive as migration-facing aliases where already used.
- Words such as `lava` and `aurora` may appear in preset or theme names today without proving that generator-family metadata already exists in the current runtime.

## Modifier Category Taxonomy

Modifiers are categories, not promises that each category already has a separate runtime object or serialized field.

| Category | Frozen meaning | Current repo note |
| --- | --- | --- |
| `drift` | slow positional or phase translation of the field over time | implicit in current phase motion, not explicit metadata |
| `turbulence` | bounded chaotic agitation of a base field | not explicit today |
| `distortion` | geometric or domain-space warping applied to the field | not explicit today |
| `diffusion` | soft spreading or smoothing across neighboring samples | current legacy `smoothing` overlaps this concept |
| `feedback_history` | controlled reuse of prior field state | current fire path uses local history, but not as a named modifier |
| `threshold_banding` | discrete bucket or band quantization before presentation | not explicit today |
| `contour_extraction` | iso-line or edge extraction from a field | not explicit today |
| `edge_emphasis` | local emphasis of edges or steep gradients | not explicit today |
| `glyph_mapping_prep` | normalization or indexing preparation for glyph output | not explicit today |
| `temporal_accumulation` | deliberate frame-to-frame accumulation | not explicit today |
| `decay_persistence` | controlled fade or retention of accumulated state | not explicit today |
| `palette_cycling_influence` | color-travel or palette-phase influence without changing generator family | implicit in current palette drift |
| `domain_warp_flow_influence` | vector or flow-driven warping of later field stages | not explicit today |

## Output Family Taxonomy

| Output family | Frozen meaning | Current status |
| --- | --- | --- |
| `raster` | direct pixel or texel-style field presentation | current classic output family |
| `banded` | bucketed or posterized field presentation where banding is the main identity | taxonomy only today |
| `contour` | quantized, iso-line, or contour-result presentation | taxonomy only today |
| `glyph` | symbol- or index-driven presentation derived from the field | taxonomy only today |
| `surface` | surface-oriented interpretation of the field before final dimensional presentation | taxonomy only today |

## Output Mode Taxonomy

| Output mode | Family | Frozen meaning | Current status |
| --- | --- | --- | --- |
| `native_raster` | `raster` | direct raster presentation with no dither-as-identity | current classic mode |
| `dithered_raster` | `raster` | raster presentation whose primary identity is dithered quantization | taxonomy only today |
| `contour_only` | `contour` | contour lines or extracted contour result without filled band regions | taxonomy only today |
| `contour_bands` | `contour` | contour result combined with discrete filled bands | taxonomy only today |
| `ascii_glyph` | `glyph` | ASCII character presentation derived from the field | taxonomy only today |
| `matrix_glyph` | `glyph` | denser glyph presentation associated with matrix-style symbol flow | taxonomy only today |
| `heightfield_surface` | `surface` | surface output whose main topology is heightfield-driven | taxonomy only today |
| `curtain_surface` | `surface` | surface output whose main topology is curtain-like sheets | taxonomy only today |
| `ribbon_surface` | `surface` | surface output whose main topology is ribbon-like strips | taxonomy only today |
| `extruded_contour_surface` | `surface` | surface output built from contour extrusion rather than plain raster height | taxonomy only today |

Rules:

- `dithered_raster` as an output mode means dither is the main output identity.
- `dither` as a treatment means a bounded sampling choice that does not redefine the whole output family by itself.
- `ascii_glyph` and `matrix_glyph` are output modes, not generators.

## Treatment-Slot Taxonomy

Every treatment slot allows `none`.
No treatment may appear outside its slot.

| Slot | Frozen role | Allowed family vocabulary | Current repo note |
| --- | --- | --- | --- |
| `sampling_treatment` | sampling and resampling character before later post work | `nearest`, `soft`, `dither` | current legacy `smoothing` only overlaps this loosely |
| `filter_treatment` | bounded post or filter work after theme mapping | `blur`, `glow_edge`, `halftone_stipple`, `kaleidoscope_mirror`, `restrained_glitch`, `emboss_edge` | no separate slot today |
| `emulation_treatment` | display-emulation character | `phosphor`, `crt` | no separate slot today; phosphor currently appears only in preset/theme naming |
| `accent_treatment` | bounded overlays or accent passes that do not replace core output identity | `overlay_pass`, `accent_pass` | no separate slot today |

Bridge note:
current `smoothing=off|soft|glow` is a legacy product control.
It is not the final PL01 treatment vocabulary and should not be treated as proof that separate treatment slots already exist in the runtime.

## Dimensional Presentation Taxonomy

| Presentation mode | Frozen meaning | Current status |
| --- | --- | --- |
| `flat` | plain 2D presentation | current classic posture |
| `heightfield` | bounded height-based depth presentation | taxonomy only today |
| `curtain` | bounded curtain-sheet presentation | taxonomy only today |
| `ribbon` | bounded ribbon-strip presentation | taxonomy only today |
| `contour_extrusion` | bounded extrusion of contour-derived forms | taxonomy only today |
| `bounded_surface` | bounded 2.5D or 3D surface presentation that remains abstract | taxonomy only today |
| `bounded_billboard` | bounded volume-like or billboard-style presentation without scenic camera behavior | taxonomy only today |

Rules:

- Presentation modes remain downstream of generator, modifier, output, and treatment choices.
- None of these modes authorize scenic worlds, free cameras, or object-demo identity.

## Theme-Family Taxonomy

A theme family is a lineage.
A concrete theme instance is one theme key within that lineage with specific roles, hints, and safety context.

| Theme family | Frozen meaning |
| --- | --- |
| `ember` | warm incandescent red-orange family with restrained ember heat rather than scenic fire realism |
| `oceanic` | blue-teal aquatic abstract family |
| `aurora` | cool luminous green-cyan-violet family |
| `toxic` | acidic or hazardous neon family with intentionally sharp chemical color posture |
| `monochrome` | single-hue or neutral monochrome family |
| `green_phosphor` | green display-emulation family |
| `amber_phosphor` | amber display-emulation family |
| `blue_phosphor` | blue display-emulation family |
| `museum_white` | pale exhibit-like neutral family |
| `noir` | deep dark, low-key, contrast-restrained family |
| `candy` | playful saturated candy-color family |
| `thermal` | heat-map style family |
| `topo` | map-like contour or elevation-color family |
| `frost` | cold pale ice-like family |
| `bioluminescent` | deep dark luminous living-light family kept abstract |
| `abyssal` | deep blue-black or trench-dark family |
| `volcanic` | molten black-red-orange family heavier than simple ember palettes |
| `ultraviolet` | UV or violet-luminous family |
| `patina` | oxidized metal green-blue family |
| `obsidian` | glassy near-black family with restrained highlights |
| `neon_noir` | dark urban neon family |
| `ash_cinder` | muted soot, cinder, and dim-ember family |

Current repo mappings are still conceptual only because built-in themes do not carry `theme_family` metadata yet.
The closest current conceptual mappings are:

- `plasma_lava` and `lava_remix`: `ember`
- `aurora_cool`: `aurora`
- `oceanic_blue` and `midnight_interference`: `oceanic`
- `museum_phosphor`: `green_phosphor`
- `amber_terminal`: `amber_phosphor`
- `quiet_darkroom`: `ash_cinder`

## Profile / Quality / Safety Taxonomy

### Profile Classes

| Profile class | Frozen meaning |
| --- | --- |
| `stable` | content intended for the honest stable core, including truthful lower-band behavior and preserved migration expectations |
| `experimental` | content intentionally isolated because identity, safety, compatibility, or presentation risk is still under evaluation |

### Quality Classes

| Quality class | Frozen meaning | Current repo note |
| --- | --- | --- |
| `safe` | conservative cost posture intended to remain honest on current lower bands | current saver manifest policy |
| `balanced` | moderate uplift posture allowed to ask for richer presentation while still degrading honestly | already used in sample pack and preset routing |
| `high` | explicitly richer posture that expects more headroom but still may not hide unsupported lower-band truth | taxonomy only today |
| `premium` | explicit top-end uplift class that must never become the hidden baseline | taxonomy only today |

### Safety Tags

| Safety tag | Frozen meaning |
| --- | --- |
| `preview_safe` | remains usable and legible in the preview path |
| `long_run_safe` | remains stable and visually bounded during unattended runs |
| `dark_room_safe` | remains restrained enough for dark-room use |
| `capture_safe` | remains suitable for deterministic or otherwise controlled capture claims |

Important distinction:
the current `presets/performance.ini` profile name `preview_safe` is a legacy performance-profile label, not proof that safety tags are already serialized as first-class metadata.
The current saver manifest also uses `long_run_stable` as a capability claim; `long_run_safe` is the canonical PL01 safety-tag term for later product-local metadata.

## Degradation Vocabulary

The canonical degrade-policy vocabulary is:

| Token | Frozen meaning |
| --- | --- |
| `drop_premium_presentation` | remove presentation work that only belongs to the premium lane |
| `drop_heavy_post` | remove expensive post-treatment work first |
| `drop_advanced_only_modifiers` | remove modifiers that are not honest below advanced bands |
| `drop_emulation_extras` | remove optional emulation embellishments while preserving the main output identity |
| `reduce_filter_quality` | keep the same filter class but at a cheaper quality level |
| `reduce_internal_resolution` | lower internal field or presentation resolution |
| `unsupported_in_band` | declare the path unsupported in the active band instead of faking it |
| `preserve_identity` | terminal honesty rule meaning the remaining path still represents the same content honestly |
| `fail_honestly` | terminal honesty rule meaning no truthful degraded version exists |

Rules:

- degrade-policy tokens are ordered
- `preserve_identity` and `fail_honestly` are terminal truth statements, not ordinary cost-reduction steps
- lower-band operation is normal, but it must remain explicit

## Transition Vocabulary

### Transition Kinds

| Transition kind | Frozen meaning |
| --- | --- |
| `theme_morph` | morph where generator/output identity stays materially the same and the appearance layer changes dominate |
| `preset_morph` | morph between two presets with sufficiently compatible structure for direct interpolation or controlled crossfade |
| `bridge_morph` | explicitly authored bridge used when a direct preset morph would not be honest |
| `journey` | a multi-step or weighted travel plan across presets, themes, and transitions |

### Morph Compatibility Classes

| Compatibility class | Frozen meaning |
| --- | --- |
| `exact` | direct morph is honest because the content topology materially matches |
| `family` | direct morph is honest within a shared family and compatible output/presentation shape |
| `bridge_required` | direct morph is not honest; a declared bridge is required |
| `cut_only` | no honest morph exists; only non-morph fallback is allowed |

### Fallback Transition Behavior

| Fallback behavior | Frozen meaning |
| --- | --- |
| `hard_cut` | switch directly without pretending a morph happened |
| `hold_then_cut` | briefly hold the outgoing state and then cut cleanly |
| `theme_only_morph` | morph only the theme layer and cut the structural state |
| `fail_honestly` | decline the transition rather than overclaim support |

Repo reality note:
none of the transition vocabulary above is implemented as a current runtime feature.
`PL01` freezes the words only.

## Naming / Keying Rules

- Canonical ids use lowercase snake_case.
- Generator, output, treatment, presentation, theme-family, policy, and transition vocabulary must stay backend-agnostic.
- Display names may remain more human-readable than canonical ids.
- Legacy aliases remain migration inputs where the repo already supports them, but new doctrine text should prefer canonical ids.
- Do not use vague buckets such as generic `mode` when a more precise term exists.
- Keep `interference` as a classic display or alias term, but use `wave` for canonical taxonomy work.

## Taxonomy-Only Today Vs Implemented Today

Implemented today:

- generator equivalents for `plasma`, `fire_like`, and `wave`
- classic `raster` output in a `native_raster` posture
- flat presentation
- saver-level `quality_class=safe`
- saver-level `preview_safe` and `long_run_stable` claims, with `long_run_safe` reserved as the canonical PL01 tag term

Taxonomy-only today:

- explicit generator-family metadata fields
- explicit modifier-set metadata
- explicit output-family and output-mode metadata
- fixed treatment slots
- dimensional presentation metadata
- theme-family metadata
- transition and journey metadata
- structured degrade-policy tokens in product files

## What PL02 / PL03 Will Consume

`PL02` should consume this freeze for:

- generator-family naming
- modifier-set naming
- output family and mode naming
- treatment-slot boundaries
- dimensional presentation vocabulary
- theme-family classification work

`PL03` should consume this freeze for:

- product-local schema and parser mapping
- set membership and selection metadata
- transition and journey metadata
- degrade-policy token handling
- migration rules from legacy effect/smoothing terms into canonical vocabulary
