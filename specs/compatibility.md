# Compatibility Specification

This document is normative.

## Supported Windows Band

The platform support goal is Windows 95, 98, and ME plus Windows NT 4.0 through Windows 11.

This is a compatibility band, not a claim that every later feature will behave identically on every system revision.
The repository must preserve a baseline execution path that remains viable on the earliest supported systems.

## Excluded Targets

The following are outside the baseline contract:

- 16-bit Windows and Win32s, because the project is defined as a Win32 platform.
- Non-Windows targets, because the host model is the classic Windows screensaver model.
- x64-only or Unicode-only baselines, because they would weaken the intended retro-compatible floor.
- DirectX-first or GPU-mandatory render paths, because GDI remains the universal floor.

Later series may add extra host binaries or optional helper targets, but they must not replace the baseline.

## Language Baseline

All baseline platform and saver code must remain compatible with C89.

This means:

- No reliance on C99-or-later language requirements in shared baseline code.
- Header and ABI design should assume conservative C compilers and older Windows SDK environments.
- Later convenience layers must not force the shared baseline above C89.

## ABI And Binary Expectations

Baseline saver products target x86 PE32 GUI executables with the `.scr` extension.

The baseline host contract assumes:

- Classic Windows screensaver command-line conventions.
- ANSI Win32 entry paths and message handling.
- No mandatory external runtime beyond what a normal Win32 application can reasonably assume on the target band.

Additional helper applications may exist later, but releasable saver products must continue to honor the `.scr` baseline.

## Renderer Policy

Renderer support is tiered:

- GDI is mandatory and universal.
- OpenGL 1.1 is optional and must be detected at runtime.
- OpenGL 2.1 is the current real later-capability tier and remains optional, capability-gated, and subordinate to the universal baseline.
- OpenGL 3.3 is the current real modern tier, but it remains optional, capability-gated, and subordinate to the universal baseline.
- OpenGL 4.6 may still be named explicitly as a later optional tier before it is implemented, but it remains a placeholder until a real backend exists.
- An internal null safety fallback may exist beneath GDI for host stability, but it does not change the required product baseline.

Later `SX` work may enrich private routing, backend, or service layers beneath this public tier ladder, but it must not replace the public ladder itself without a later explicit doctrine change.

No product may rely on a renderer tier that is unavailable on the baseline machine unless that product is explicitly designated later as an enhanced-only product class.

## Capability Rules

The repository must follow these rules:

- Universal behavior ships first.
- Enhancements layer on top of the universal path.
- If an optional capability is absent, the product must either fall back cleanly or be classified as not part of the universal saver set.
- Product releases must state when they depend on a capability above the universal floor.
- Future private service seams must remain dynamically optional and must not become new mandatory baseline runtime requirements.

## Compatibility Philosophy

The project prioritizes the widest durable baseline first and visual enhancements second.

A feature is not acceptable merely because it works well on modern systems.
It must either preserve the baseline or be clearly isolated as optional.

Private substrate growth is allowed.
Compatibility-floor replacement is not.

## Early-Series Non-Goals

Early series are not trying to:

- Maximize modern GPU features.
- Standardize on Unicode-only UI paths.
- Introduce mandatory middleware or third-party runtimes.
- Replace the Win32 `.scr` model with a different application model.
- Treat D3D, D2D, or DWrite as already-adopted public first-class tiers during `SX00`.
