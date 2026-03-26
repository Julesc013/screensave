# Compatibility

## Baseline

- Language baseline: C89.
- Product baseline: x86 Win32 `.scr`.
- Required renderer floor: GDI.

## Optional Tiers

- GL11 is the conservative optional acceleration tier.
- Any renderer beyond GL11 is strictly optional.
- Higher tiers must be capability-gated and must not become required for baseline functionality.

## Guardrails

- Do not silently raise the language baseline.
- Do not silently move the baseline target away from Win32 screensavers.
- Do not make GDI an afterthought or a fallback that is no longer maintained.
- Do not expand shared APIs for a single product without strong justification.
