# Win32 Saver Host

Purpose: classic ANSI Win32 `.scr` host implementation.
Belongs here: screensaver mode routing, preview integration, and saver-host window management.
Does not belong here: product rendering logic or packaging behavior.
Current stage: Series 03 introduced the first real host skeleton here:

- classic `/s`, `/p`, and `/c` mode routing
- a full-screen saver window path
- a preview child-window path
- a small configuration dialog and provisional settings persistence
- a temporary host-local liveness path that was used before the shared renderer existed

Series 04 lightly adopted the shared core here for:

- saver/module identity and entry validation
- shared version text and diagnostics state
- shared common-config and deterministic-seed scaffolding
- shared timing helpers

Series 05 routes screen and preview rendering through the reusable GDI backend.
Series 06 keeps the host lifecycle intact while loading real product-owned config, delegating config mode to the active saver module when it provides a dialog, and running Nocturne as the first real saver product.
Series 07 shares the public saver-config state and window-bound renderer helpers with BenchLab while leaving the `.scr` lifecycle itself private to this host.
Series 08 keeps the host on automatic shared-renderer selection so GDI remains the guaranteed floor while the optional GL11 path can be exercised honestly without turning the host config UI into a renderer lab.
Series 09 adds a narrow built-in saver selector for the current real saver set, persists the selected product key at the host level, and keeps product-specific settings inside each saver's own dialog instead of turning the host UI into a suite manager.
Series 12 extends the same explicit built-in selector to the current ten-product saver set, including Signals, Mechanize, and Ecosystems, while preserving the existing host lifecycle and automatic renderer-selection behavior.
Series 13 extends the same explicit built-in selector to the current thirteen-product saver set, including Stormglass, Transit, and Observatory, while preserving the existing host lifecycle and automatic renderer-selection behavior.
Series 14 extends the same explicit built-in selector to the current seventeen-product saver set, including Vector, Explorer, City, and Atlas, while preserving the existing host lifecycle and automatic renderer-selection behavior.
Series 15 keeps the selector narrow while adding persistence for the requested renderer ladder (`auto`, `gdi`, `gl11`, `gl21`, `gl33`, or `gl46`) and surfacing requested-versus-active renderer diagnostics without turning the host config UI into the future `suite` surface or a renderer lab.
C05 hardens the real `.scr` lifecycle for the standalone saver line by:

- keeping preview mode as a true child-window path while revalidating the preview parent and resynchronizing preview size during live preview
- adding a bounded single-saver `/c` shell so standalone savers expose renderer preference plus shared common settings before delegating to the owning saver dialog
- sizing fullscreen saver windows against the Windows virtual desktop when multiple monitors are reported instead of assuming only the primary screen
- resynchronizing preview and fullscreen window bounds on `WM_DISPLAYCHANGE`
- documenting the current multi-monitor policy as one saver window spanning the virtual desktop, not independent per-monitor saver sessions

The validation scene remains only as a fallback path for non-product sessions.
Type: runtime host implementation.
