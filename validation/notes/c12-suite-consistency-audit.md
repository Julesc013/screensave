# C12 Suite Consistency Audit

`C12` is the bounded cross-cutting polish pass that normalizes the shared outer surface before any saver-specific deep polish begins.

## Scope Locked In

This prompt stayed inside cross-cutting polish scope:

- no new saver-local deep features
- no renderer-tier expansion
- no packaging or installer expansion
- no new product-family work

## Normalized In C12

The following suite-wide consistency fixes are now active:

| Area | C12 normalization |
| --- | --- |
| Host shell wording | `Renderer preference` and `Session randomization` are now the canonical shared host labels |
| Suite app actions | `Apply` persists the current working copy and `Revert Changes` reloads the saved config without claiming a defaults reset |
| Preset and theme display | `suite` now shows preset and theme display names when descriptors provide them, while diagnostics surfaces keep stable keys where appropriate |
| Renderer vocabulary | Host diagnostics, BenchLab, and `suite` now use `Renderer preference`, `Active renderer`, `Selection path`, `Fallback cause`, and `Renderer status` with human-readable text instead of raw internal codes |
| Detail and randomization semantics | Diagnostics and info panes now consistently use `Detail level` plus `Randomization mode`, while end-user toggles keep the simpler `Session randomization` wording |
| Family and renderer support labels | `suite` now presents family names and renderer support with user-facing text instead of raw manifest tokens such as `gdi_gl11` |
| Meta-saver config wording | `anthology` now uses `Renderer policy` and `Honor saver-local randomization` in its product-owned dialog |
| BenchLab metadata | BenchLab now truthfully advertises the full current saver line, including `anthology`, in its manifest |

## Intentional Individuality Preserved

`C12` did not flatten the suite into one generic UI.
The following remain product-local by design:

- saver-owned dialog layouts
- saver-owned preset and theme catalogs
- product-local summaries and descriptions
- anthology selection semantics
- BenchLab diagnostics depth

## Quality-Bar Hand-Off

The durable rules for later saver-specific polish waves now live in `docs/roadmap/c12-suite-quality-bar.md`.

## Next Step

- `C13` first saver-specific polish wave
