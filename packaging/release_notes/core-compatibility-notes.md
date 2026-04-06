# ScreenSave Core Compatibility Notes

This note records the compatibility framing for the frozen `C16` ScreenSave Core baseline.

## Artifact Floor

- Baseline saver artifact: x86 Win32 `.scr`
- Host model: classic ANSI Win32 screen saver path
- Guaranteed renderer floor: `gdi`

## Optional Capability Tiers

- `gl11` remains optional and capability-gated.
- `gl21` remains optional and capability-gated.
- `gl33` and `gl46` remain named placeholders, not real shipped requirements.

## Product-Class Boundaries

- Core is the standalone saver ZIP only.
- Core does not require the Installer channel.
- Core does not require `suite`, BenchLab, the SDK, or Extras content.
- `anthology` remains a standalone saver product inside Core, not a replacement for the separate saver line.

## Evidence Level

- The `C16` baseline uses a fresh `Release|Win32` build from `build/msvc/vs2022/ScreenSave.sln` on April 6, 2026.
- Shared Windows host lifecycle expectations continue to rely on the recorded `C05` validation note plus later source-level hardening and release-build evidence.
- This freeze does not claim identical runtime behavior on every supported Windows revision without further machine-specific validation.

## Current Installer Compatibility Notes

- The companion Installer channel remains current-user only.
- Machine-wide install and richer repair modes remain deferred after `C16`.
