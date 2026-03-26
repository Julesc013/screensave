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
The validation scene remains only as a fallback path for non-product sessions.
Type: runtime host implementation.
