# C12 Suite Quality Bar

This document defines the suite-wide quality bar that applies before and during the later saver-specific polish waves.

Use it to preserve one coherent product family while still allowing each saver to keep its own identity.

## Shared Outer Grammar

The suite now treats the following shared labels as canonical:

| Concern | User-facing term | Diagnostics-facing term | Notes |
| --- | --- | --- | --- |
| Renderer request | `Renderer preference` | `Renderer preference` | Use `Auto (best available)` for the automatic path, `GDI floor` for the universal baseline, and `OpenGL 1.1` / `OpenGL 2.1` for current optional tiers |
| Active backend result | `Active renderer` | `Active renderer` | Human-readable names only; do not expose raw `gl11` / `gl21` tokens directly in end-user surfaces |
| Selection mechanics | not usually shown | `Selection path` | Use for renderer-selection, fallback-routing, or anthology-selection diagnostics |
| Failure reason | `Fallback cause` when surfaced | `Fallback cause` | Humanize raw renderer and fallback codes before surfacing them |
| Renderer health | `Renderer status` | `Renderer status` | Use bounded, truthful wording such as `Ready`, `Double-buffered`, or `Running on the GDI floor` |
| Detail control | `Detail level` | `Detail level` | Canonical values are `Low`, `Standard`, and `High` |
| Randomization toggle | `Session randomization` | `Randomization mode` | End-user toggles stay simple; diagnostics may report `Off` or `Session` explicitly |
| Preset selection | `Preset` / `Selected preset` | `Preset key` | End-user surfaces prefer display names; diagnostics may show stable keys |
| Theme selection | `Theme` / `Selected theme` | `Theme key` | End-user surfaces prefer display names; diagnostics may show stable keys |

When a product-owned dialog or app surface needs higher-level grouping, prefer this bounded outer structure:

- `General`
- `Appearance`
- `Behavior`
- `Randomization`
- `Advanced`

Products do not need to expose every section.
Use these names only when the shared concern is truly present.

## Action Semantics

Use these action terms consistently:

| Action | Meaning | Where It Applies |
| --- | --- | --- |
| `Apply` | Persist the current app-owned working copy | `suite` and other bounded multi-control app surfaces |
| `Revert Changes` | Reload the last saved state without claiming a default reset | `suite` and any later app-owned editor surface |
| `Defaults` | Reset the current product-owned dialog to product defaults | Saver-owned dialogs and the shared host shells |
| `Import` / `Export` | Move versioned preset, theme, or pack data across the shared file formats | Product-owned config and later bounded data-management surfaces |
| `Settings...` | Open the product-owned configuration dialog | Host and `suite` handoff points only |

Do not reuse `Reset` to mean both "reload saved state" and "reset to defaults".

## Preset And Theme Taxonomy

The suite-wide naming model is:

- Preset display names use title-style display text and stay short enough for combo boxes and list views.
- Theme display names use title-style display text and should read like an appearance bundle, not a behavior mode.
- Stable keys remain lowercase machine identifiers owned by the product.
- End-user surfaces should show display names when descriptors provide them.
- Diagnostics surfaces may show keys when stable identity matters more than presentation.
- Presets and themes are distinct concepts: presets bundle behavior plus shared common settings, while themes describe appearance or palette direction.

Do not collapse presets and themes into one generic "mode" label.

## Metadata And Product Identity

The following identity rules now apply across savers, `anthology`, `suite`, and BenchLab:

- Canonical slugs remain the source of truth for product identity and migration.
- Display names should be human-readable and stable, but they must not contradict the canonical slug.
- Manifest summaries, about text, and resource descriptions should describe the current role of the product, not an older stage of the series.
- `suite`, `anthology`, and BenchLab must keep their role boundaries explicit in manifests and docs.

## Renderer And Compatibility Truth

The suite-wide compatibility grammar is:

- `GDI floor` means the universal baseline path that must remain available.
- `OpenGL 1.1` and `OpenGL 2.1` remain optional, capability-gated tiers.
- `OpenGL 3.3 (placeholder)` and `OpenGL 4.6 (placeholder)` remain placeholder requests only.
- The internal `null` backend stays diagnostics-only and must never be presented as a normal product capability tier.
- Fallback wording must explain what happened without overclaiming capability.

Do not introduce raw internal failure tokens into end-user copy when a bounded human-readable translation exists.

## Multi-Monitor And Migration Wording

Current multi-monitor wording is locked to the implemented `C05` behavior:

- one saver window spanning the current virtual desktop
- no independent per-monitor saver sessions yet

Migration and recovery wording should stay explicit:

- `Stored saver settings could not be loaded; product defaults are shown for editing.`
- `Saver settings and renderer preference could not be saved.`
- invalid preset, theme, or pack imports should fail safely without partial apply

Do not imply a larger migration or multi-monitor system than the repo currently implements.

## What Later Saver Waves May Change

Later saver-specific waves may still change:

- product-local preset sets
- theme sets and display names
- default preset or theme choices
- product-local summaries and dialog grouping
- pacing, long-run tuning, and product-specific art direction

## What Later Saver Waves Must Preserve

Later saver-specific waves must preserve:

- canonical slugs and migration rules
- the shared outer grammar in this document
- truthful renderer and fallback wording
- the current role split among standalone savers, `anthology`, `suite`, BenchLab, and the SDK
- the compatibility baseline and bounded shared settings contract

## Next Step

`C12` establishes the suite-consistency baseline.
`C13` Wave A and Wave B have already applied that baseline to the first ten polished savers.
`C13` Wave C and `C14` final rerelease hardening completed the continuation line without reopening the quality-bar vocabulary.
Later post-release work should continue to preserve this quality bar.
