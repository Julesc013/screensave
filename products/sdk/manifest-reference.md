# Manifest Reference

This is the contributor-facing quick reference for the current authoring formats.

## Saver Manifest

Path:

- `products/savers/<slug>/manifest.ini`

Required sections:

- `[product]`
- `[identity]`
- `[capabilities]`

Recommended section:

- `[notes]`
- `[routing]`

Minimum fields:

```ini
[product]
key=<slug>
name=<Display Name>
kind=saver
version=0.0.0

[identity]
summary=<short product summary>
default_preset=<preset_key>
default_theme=<theme_key>

[capabilities]
gdi=1
gl11=0
gl21=0
preview_safe=1
long_run_stable=1

[routing]
minimum_kind=gdi
preferred_kind=gl11
quality_class=safe
```

Keep capability claims honest.
Keep `[routing]` aligned with the compiled saver module policy used at runtime.

Shared Wave 4 reference vocabulary for saver identity, default exposure, and stable-versus-experimental visibility is frozen in [`docs/roadmap/sy40-sdk-reference-contract.md`](../../docs/roadmap/sy40-sdk-reference-contract.md).
That contract covers only the shared outer shell.
Preset meaning, theme meaning, favorites, journeys, provenance, and automation remain product-local unless later promoted explicitly.

## Built-In Saver Preset Catalog Files

Path shape:

- `products/savers/<slug>/presets/*.ini`

These files are not the same as exported `.preset.ini` files.
They are product-owned catalog sources checked into the saver tree.
They use repeated `[preset]` blocks such as:

```ini
[preset]
key=starter_drift
theme=starter_blue
detail=standard
motion=drift
accent=frame
spacing=balanced
```

The exact keys after `key`, `theme`, and `detail` remain product-local.
Keep those keys aligned with the saver's config enums, descriptor tables, and defaults.

## Preset Export / Import File

Path shape:

- `<pack-root>/presets/<name>.preset.ini`

Required sections:

- `[format]`
- `[product]`
- `[common]`

Minimum fields:

```ini
[format]
kind=preset
version=1

[product]
key=<slug>
schema_version=1

[common]
detail_level=standard
seed_mode=session
deterministic_seed=0
diagnostics_overlay_enabled=0
randomization_mode=off
randomization_scope=none
preset_key=<preset_key>
theme_key=<theme_key>
```

Add saver-owned keys under `[product]`.
Optional routing intent may be recorded under:

```ini
[routing]
minimum_kind=gdi
preferred_kind=gl11
quality_class=safe
```

This metadata is advisory unless the importing runtime surface treats the preset file itself as the active routing input.

## Theme Export / Import File

Path shape:

- `<pack-root>/themes/<name>.theme.ini`

Required sections:

- `[format]`
- `[product]`
- `[theme]`

Minimum fields:

```ini
[format]
kind=theme
version=1

[product]
key=<slug>
schema_version=1

[theme]
theme_key=<theme_key>
display_name=<Theme Display Name>
description=<short description>
primary_color=#AABBCC
accent_color=#112233
```

## Pack Manifest

Path:

- `<pack-root>/pack.ini`

Required sections:

- `[pack]`
- `[files]`

Minimum fields:

```ini
[pack]
format=screensave-pack
version=1
schema_version=1
pack_key=<pack_key>
product_key=<slug>
display_name=<Pack Display Name>
description=<short description>

[files]
preset_1=presets/<preset>.preset.ini
theme_1=themes/<theme>.theme.ini
```

Use only safe relative paths.
Do not use absolute paths or `..`.
Optional routing intent may be recorded under:

```ini
[routing]
minimum_kind=gdi
preferred_kind=gl11
quality_class=safe
```

This metadata is advisory unless the consuming runtime surface actively routes through that pack artifact.
