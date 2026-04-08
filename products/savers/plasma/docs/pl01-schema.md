# Plasma PL01 Schema Direction

Status: product-local logical schema freeze after `PL00`.

This document freezes the logical content schema Plasma wants.
It does not claim that the current parser, runtime, or UI already reads every field defined here.

## Purpose

This document exists to:

- keep shared outer config law intact
- freeze the product-local logical fields Plasma wants next
- distinguish current repo-supported forms from future logical schema
- give later parser, runtime, migration, settings, and content work one stable target

## Relationship To Shared `specs/config_schema.md`

[`../../../../specs/config_schema.md`](../../../../specs/config_schema.md) remains the shared outer law for:

- `.preset.ini`
- `.theme.ini`
- `pack.ini`
- shared common config fields
- shared routing fields

`PL01` does not rewrite that spec.
Instead, it freezes the Plasma-local logical model that should sit inside or beside the shared outer envelope.

Key distinction:

- the shared spec defines the cross-product envelope
- this document defines the Plasma-local resolved model

## Logical Model Versus Current Serializations

In this document:

- `logical field` means the resolved product meaning Plasma wants later `PL` work to understand
- `required` means required in the resolved logical model, not necessarily already required in every current text file
- current text files may omit fields and inherit from saver defaults, preset defaults, pack routing, or shared common state

## Current Repo Forms Vs Target Logical Schema

| Current form | Current repo support | Relation to target logical schema |
| --- | --- | --- |
| compiled preset descriptors in `plasma_presets.c` | `preset_key`, `display_name`, `description`, `theme_key`, `detail_level`, `use_fixed_seed`, `fixed_seed` | partial built-in preset model only |
| compiled theme descriptors in `plasma_themes.c` | `theme_key`, `display_name`, `description`, `primary_color`, `accent_color` | partial built-in theme model only |
| repo preset lists in `presets/defaults.ini` and `presets/themed.ini` | legacy source keys such as `effect`, `speed`, `resolution`, `smoothing`, sometimes `detail` | authoring convenience, not the full future schema |
| repo performance profiles in `presets/performance.ini` | legacy performance-profile labels and tuning values | not the same thing as PL01 profile classes or safety tags |
| shared preset export/import files | shared `[format]`, `[product]`, `[common]`, `[routing]` plus parsed product keys for effect/speed/resolution/smoothing | current import/export contract for a narrow classic subset |
| shared theme export/import files | shared `[format]`, `[product]`, `[theme]` with appearance values | current theme contract for a narrow appearance subset |
| pack manifests | shared `[pack]`, `[routing]`, `[files]` | current data-bundle envelope only |
| preset sets, theme sets, transition sets, journeys | no current file or parser support | target logical schema only |

## Canonical Logical Schema Conventions

- Keys use lowercase snake_case.
- Logical selectors point to stable content keys, not display names.
- Product-local fields should remain backend-agnostic.
- Logical fields may be resolved through inheritance even when serialized forms omit them.
- Legacy aliases remain valid migration inputs, but canonical writes should prefer canonical keys.

## Canonical Logical Schema: Preset

Preset fields are resolved product meaning.
Current serializations only support a subset.

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `preset_key` | yes | stable preset identity | yes |
| `display_name` | yes | human-readable preset name | yes in compiled descriptors, not in built-in preset list INIs |
| `summary` | yes | short honest preset description | yes in compiled descriptors, not in current preset export files |
| `generator_family` | yes | canonical generator family id | no explicit field today |
| `modifier_set` | yes | ordered modifier ids or named modifier-set reference; `none` if empty | no explicit field today |
| `output_family` | yes | canonical output family | no explicit field today; current classic path is implicit `raster` |
| `output_mode` | yes | canonical output mode | no explicit field today; current classic path is implicit `native_raster` |
| `theme_key` | yes | bound concrete theme identity | yes |
| `sampling_treatment` | yes | one treatment id for the sampling slot or `none` | no explicit field today |
| `filter_treatment` | yes | one treatment id for the filter slot or `none` | no explicit field today |
| `emulation_treatment` | yes | one treatment id for the emulation slot or `none` | no explicit field today |
| `accent_treatment` | yes | one treatment id for the accent slot or `none` | no explicit field today |
| `presentation_mode` | yes | canonical dimensional presentation mode | no explicit field today; current classic posture is implicit `flat` |
| `profile_class` | yes | `stable` or `experimental` | no explicit field today |
| `quality_class` | yes | resolved quality class after inheritance | partial through shared `[routing]` |
| `minimum_kind` | yes | resolved minimum public renderer kind after inheritance | partial through shared `[routing]` |
| `preferred_kind` | yes | resolved preferred public renderer kind after inheritance | partial through shared `[routing]` |
| `degrade_policy` | yes | ordered degrade-policy tokens plus terminal honesty rule | no structured field today; only free-text routing notes |
| `preview_safe` | yes | preset-level safety tag | no explicit field today |
| `long_run_safe` | yes | preset-level safety tag | no explicit field today |
| `dark_room_safe` | yes | preset-level safety tag | no explicit field today |
| `capture_safe` | yes | preset-level safety tag | no explicit field today |
| `seed_policy` | yes | `inherit`, `fixed`, or `range_derived` | partial today through common seed settings and compiled fixed-seed descriptors |
| `fixed_seed` | optional | required when `seed_policy=fixed` | partial today |
| `set_membership` | optional | preset-set membership references | no explicit field today |
| `selection_weight` | optional | weighting for randomization, sets, or journeys | no explicit field today |
| `parameters` | optional | product-local parameter block | partial today through product `effect`, `speed`, `resolution`, `smoothing` |

PL00-derived optional logical extensions reserved here:

| Field | Meaning | Current repo support |
| --- | --- | --- |
| `band_intent` | product-local band targeting hint for later uplift-aware content | no explicit field today |
| `morph_compatibility_class` | default morph compatibility class for this preset | no explicit field today |

## Canonical Logical Schema: Theme

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `theme_key` | yes | stable theme identity | yes |
| `display_name` | yes | human-readable theme name | yes |
| `summary` | yes | short honest theme description | yes as `description` today |
| `theme_family` | yes | canonical theme-family id | no explicit field today |
| `palette_roles` | yes | named palette or material roles such as `primary`, `accent`, `background`, `shadow`, `glow`, or `glyph` | partial today with only `primary_color` and `accent_color` |
| `brightness_hint` | optional | bounded brightness intent for selection and dark-room handling | no explicit field today |
| `contrast_hint` | optional | bounded contrast intent for selection and capture handling | no explicit field today |
| `safety_hints` | optional | optional theme-level hints composed from safety-tag vocabulary; they do not override preset truth alone | no explicit field today |

Rules:

- Theme metadata shapes appearance and hints.
- Theme metadata does not replace generator family, output family, or presentation mode.
- Theme-level safety hints may inform selection, but preset-level resolved safety tags remain authoritative for behavior claims.

## Canonical Logical Schema: Pack

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `pack_key` | yes | stable pack identity | yes |
| `display_name` | yes | human-readable pack name | yes |
| `summary` | yes | short honest pack description | yes as `description` today |
| `profile_class` | optional | pack-wide stable or experimental scope | no explicit field today |
| `channel_intent` | optional | packaging or surfacing intent when profile class alone is insufficient | no explicit field today |
| `included_presets` | optional | included preset references | yes indirectly through `[files]` |
| `included_themes` | optional | included theme references | yes indirectly through `[files]` |
| `included_transition_sets` | optional | included transition-set or journey references | no explicit field today |
| `minimum_kind` | optional | pack-level routing floor override or note | yes through shared `[routing]` |
| `preferred_kind` | optional | pack-level preferred lane | yes through shared `[routing]` |
| `quality_class` | optional | pack-level routing quality note | yes through shared `[routing]` |
| `band_notes` | optional | product-local note about band expectations | no explicit field today |
| `compatibility_notes` | optional | bounded compatibility explanation | partial today through free-text routing notes |

## Canonical Logical Schema: Preset Set

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `set_key` | yes | stable preset-set identity | no current support |
| `display_name` | yes | human-readable set name | no current support |
| `summary` | optional | short honest set description | no current support |
| `preset_members` | yes | included preset keys | no current support |
| `weighting` | optional | relative preset weighting inside the set | no current support |
| `profile_scope` | optional | whether the set is stable-only, experimental-only, or mixed | no current support |
| `morph_compatibility_class` | optional | dominant compatibility expectation for the set | no current support |
| `journey_intent` | optional | whether the set is intended for randomization, journeys, or manual selection | no current support |
| `exclusions` | optional | preset keys or tags excluded from this set | no current support |
| `constraints` | optional | product-local constraints such as requiring certain safety tags | no current support |

## Canonical Logical Schema: Theme Set

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `set_key` | yes | stable theme-set identity | no current support |
| `display_name` | yes | human-readable set name | no current support |
| `summary` | optional | short honest set description | no current support |
| `theme_members` | yes | included theme keys | no current support |
| `weighting` | optional | relative theme weighting inside the set | no current support |
| `profile_scope` | optional | whether the set is stable-only, experimental-only, or mixed | no current support |
| `journey_intent` | optional | whether the set is intended for randomization, journeys, or manual selection | no current support |
| `exclusions` | optional | theme keys or tags excluded from this set | no current support |
| `constraints` | optional | product-local constraints such as `dark_room_safe` requirements | no current support |

## Canonical Logical Schema: Transition Set / Journey

`transition_set` and `journey` remain logical-only at `PL01`.
The canonical unit inside them is a transition entry.

### Transition Entry

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `transition_kind` | yes | `theme_morph`, `preset_morph`, or `bridge_morph` | no current support |
| `from_selector` | yes | source preset, theme, or set reference | no current support |
| `to_selector` | yes | destination preset, theme, or set reference | no current support |
| `morph_compatibility_class` | yes | `exact`, `family`, `bridge_required`, or `cut_only` | no current support |
| `fallback_behavior` | yes | `hard_cut`, `hold_then_cut`, `theme_only_morph`, or `fail_honestly` | no current support |
| `bridge_key` | optional | bridge asset or rule reference for `bridge_morph` | no current support |
| `weight` | optional | transition weighting in random or journey selection | no current support |

### Transition Set / Journey Container

| Field | Required in resolved model | Meaning | Current repo support |
| --- | --- | --- | --- |
| `set_key` | yes | stable transition-set or journey identity | no current support |
| `display_name` | yes | human-readable name | no current support |
| `summary` | optional | short honest description | no current support |
| `entries` | yes | one or more transition entries | no current support |
| `profile_scope` | optional | stable-only, experimental-only, or mixed | no current support |
| `weighting` | optional | selection weighting or step weighting policy | no current support |
| `constraints` | optional | safety, band, or content constraints | no current support |
| `journey_intent` | optional | whether the container is random, ordered, cyclical, or bridge-focused | no current support |

## Seed-Policy Semantics

The canonical `seed_policy` vocabulary is:

| Value | Meaning |
| --- | --- |
| `inherit` | use the resolved shared/session seed policy without a preset-owned fixed seed |
| `fixed` | use `fixed_seed` as the preset's deterministic seed |
| `range_derived` | derive a deterministic seed from product-local parameter ranges or selection ranges and record the resolved result when capture truth requires it |

Rules:

- `fixed_seed` is only valid when `seed_policy=fixed`
- `range_derived` belongs to the logical schema even though the current runtime does not implement it yet
- current repo support is partial and split across compiled preset descriptors plus shared common seed fields

## Favorites / Exclusions / Set-Membership Direction

PL01 freezes these rules:

- favorites and exclusions are first-class at the Plasma product level
- they do not become shared suite law at `PL01`
- user-owned favorites and exclusions should point at stable preset, theme, set, or journey keys
- content-owned metadata may expose `set_membership`, `selection_weight`, `exclusions`, and `constraints`
- user-owned favorite state remains later implementation work; it is not already represented by the current shared schema

## Transition / Morph Metadata Direction

PL01 freezes these rules:

- transition truth is explicit metadata, not an implicit side effect of randomization
- morph compatibility is a stable vocabulary, not an ad hoc runtime guess
- presets and sets may later declare `morph_compatibility_class`
- transition sets and journeys own `transition_kind`, selectors, compatibility, and fallback behavior
- no current parser or engine support is implied by this freeze

## Degrade-Policy Metadata Direction

The logical `degrade_policy` field is an ordered list of tokens from the PL01 taxonomy.

Example resolved policy:

`drop_heavy_post,reduce_internal_resolution,preserve_identity`

Rules:

- `degrade_policy` is structured in the logical model
- current shared text forms only expose free-text `[routing] degraded_behavior` and optional `unsupported_paths`
- until later implementation lands, prose notes remain the only current on-disk expression of degrade behavior

## Safety-Tag Metadata Semantics

Resolved preset safety tags are booleans or boolean-equivalent tags:

- `preview_safe`
- `long_run_safe`
- `dark_room_safe`
- `capture_safe`

Rules:

- preset-level safety tags carry the authoritative behavior claim
- theme-level safety hints may inform selection but do not override preset truth alone
- current repo support remains partial: saver-level manifest capabilities already claim preview safety and long-run stability, but per-preset safety metadata is not currently parsed or enforced
- the current manifest term `long_run_stable` should be treated as the current repo precursor to the canonical logical tag `long_run_safe`, not as proof that the new tag already exists on disk

## Key / Tag Naming Rules

- canonical keys use lowercase snake_case
- use precise nouns such as `generator_family` or `presentation_mode` instead of vague `mode` buckets
- keep renderer or backend trivia out of product-local keys
- treat legacy aliases as migration inputs, not new canonical outputs
- keep display names human-readable and independent from canonical keys

## Worked Examples

### Current Repo-Supported Preset Export

This is representative of the current shared outer preset format Plasma already uses today:

```ini
[format]
kind=preset
version=1

[product]
key=plasma
schema_version=1
effect_mode=fire
speed_mode=lively
resolution_mode=fine
smoothing_mode=glow

[common]
detail_level=high
seed_mode=session
deterministic_seed=0
diagnostics_overlay_enabled=0
randomization_mode=off
randomization_scope=none
preset_key=plasma_lava
theme_key=plasma_lava

[routing]
minimum_kind=gdi
preferred_kind=gl11
quality_class=balanced
degraded_behavior=Retains the lava studies on lower lanes with coarser motion and composition updates.
```

That file is real current support.
It does not yet express generator family, output family, treatment slots, profile class, explicit safety tags, or structured degrade-policy tokens.

### Future Logical Preset Example

This example is pseudo-INI for the target logical schema only.
It is not a claim that the current loader already supports these fields.

```ini
[format]
kind=preset
version=1

[product]
key=plasma
schema_version=1

[common]
detail_level=high
seed_mode=session
preset_key=aurora_plasma
theme_key=aurora_cool

[routing]
minimum_kind=gdi
preferred_kind=gl11
quality_class=balanced

[plasma_preset]
display_name=Aurora Plasma
summary=Cool multi-wave plasma tuned for richer internal color motion.
generator_family=plasma
modifier_set=palette_cycling_influence
output_family=raster
output_mode=native_raster
sampling_treatment=soft
filter_treatment=none
emulation_treatment=none
accent_treatment=none
presentation_mode=flat
profile_class=stable
degrade_policy=reduce_internal_resolution,preserve_identity
preview_safe=1
long_run_safe=1
dark_room_safe=1
capture_safe=1
seed_policy=fixed
fixed_seed=2577
set_membership=stable_core,aurora_family
parameters.effect_mode=plasma
parameters.speed_mode=standard
parameters.resolution_mode=fine
parameters.smoothing_mode=glow
```

### Future Logical Journey Example

This example is also pseudo-INI.
It freezes desired logical direction only.

```ini
[format]
kind=journey
version=1

[product]
key=plasma
schema_version=1

[plasma_journey]
set_key=night_shift
display_name=Night Shift
summary=Slow dark-room travel across classic stable presets.
profile_scope=stable
weighting=even
constraints=require_dark_room_safe
journey_intent=ordered

[entry_1]
transition_kind=preset_morph
from_selector=museum_phosphor
to_selector=midnight_interference
morph_compatibility_class=family
fallback_behavior=hard_cut

[entry_2]
transition_kind=theme_morph
from_selector=midnight_interference
to_selector=amber_terminal
morph_compatibility_class=bridge_required
fallback_behavior=hold_then_cut
bridge_key=night_bridge_01
```

## What Is Logical-Only Today

Logical-only today:

- generator-family fields
- modifier-set fields
- explicit output-family and output-mode fields
- treatment-slot fields
- presentation-mode fields
- profile-class fields
- structured degrade-policy tokens
- safety-tag fields at the preset level
- set, transition-set, and journey schemas
- band-intent and morph-compatibility extensions

Already representable today in some form:

- `preset_key`
- `theme_key`
- shared common seed settings
- built-in fixed-seed preset descriptors
- saver and pack routing fields
- product-local effect/speed/resolution/smoothing parameters
- compiled built-in display names and summaries
