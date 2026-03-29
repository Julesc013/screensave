# C05 Windows Integration Matrix

This note records the `C05` Windows screen saver lifecycle audit and hardening pass.

## Validation Scope

- The checked-in Win32 `.scr` host was audited and hardened for real `/s`, `/p`, and `/c` invocation paths.
- Full native Control Panel / `desk.cpl` / shipped `.scr` runtime smoke execution was not rerun in this environment.
- The matrix below therefore records static host-path validation plus the concrete hardening landed in code, not fake end-user runtime evidence.

## Host-Level Hardening Landed In C05

- Single-saver `/c` now opens a bounded host shell that exposes renderer preference plus shared common settings before delegating to the owning saver's `Settings...` dialog.
- Preview mode remains a real child-window path and now revalidates the preview parent and resynchronizes child size during the live preview session.
- Fullscreen mode now sizes the saver window against the virtual desktop when Windows reports multiple monitors instead of assuming only the primary screen.
- `WM_DISPLAYCHANGE` now resynchronizes preview and fullscreen bounds so display-topology changes do not leave stale drawable sizing behind.
- Per-saver settings, requested renderer preference, and the canonical saver identity continue to persist through the real `.scr` host path without widening into a suite UI.

## BenchLab Boundary In C05

- BenchLab remains the separate diagnostics harness from earlier phases.
- Existing requested-versus-active renderer diagnostics and resolved settings inspection remain the primary developer-facing support for this phase.
- No suite-style browse, launcher, or cross-saver orchestration behavior was added here.

## Per-Saver Validation Matrix

| Saver | Screen / Fullscreen | Preview | Config / Settings | Persistence / Migration | Renderer Notes | Multi-Monitor Notes | Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `nocturne` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `ricochet` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `deepfield` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `plasma` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained, including the narrow legacy `ember_lava` alias | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `phosphor` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `pipeworks` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `lifeforms` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `signals` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `mechanize` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `ecosystems` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `stormglass` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `transit` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `observatory` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `vector` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `explorer` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `city` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `atlas` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |
| `gallery` | Shared host path audited; virtual-desktop fullscreen sizing now applied | Shared preview child-window path audited; parent revalidation and resize sync landed | Single-saver shell plus product dialog path audited | Canonical saver key plus existing migration-safe config load/save path retained | `gdi` floor, optional `gl11`, real optional `gl21`, explicit fallback reporting through shared host path | Single popup spans the virtual desktop when Windows reports multiple monitors | Native Control Panel smoke not rerun here |

## Multi-Monitor Policy In C05

- Current screen-mode behavior is one saver window spanning the Windows virtual desktop.
- Current preview behavior is one child window embedded in the Windows preview parent.
- Independent per-monitor saver sessions, monitor-specific renderer selection, and monitor-aware layout policy are intentionally deferred.
- The current hardening goal is correctness and bounded behavior on the real classic `.scr` lifecycle, not a new multi-monitor feature framework.

## Deferred After C05

- Portable distribution bundle and final on-disk bundle layout
- Installer, registration, and uninstall flow
- Suite-level browsing, orchestration, and cross-saver randomization
- Native end-to-end packaged `.scr` smoke execution outside this repository environment

## Next Continuation Step

- `C06` portable distribution bundle
