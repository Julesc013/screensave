# Win32 Saver Host

Purpose: classic ANSI Win32 `.scr` host implementation.
Belongs here: screensaver mode routing, preview integration, and saver-host window management.
Does not belong here: product rendering logic or packaging behavior.
Current stage: Series 03 introduced the first real host skeleton here:

- classic `/s`, `/p`, and `/c` mode routing
- a full-screen saver window path
- a preview child-window path
- a small configuration dialog and provisional settings persistence
- a host-local placeholder visual path used only to prove liveness before the shared renderer exists

Series 04 lightly adopts the shared core here for:

- saver/module identity and entry validation
- shared version text and diagnostics state
- shared common-config and deterministic-seed scaffolding
- shared timing helpers

The placeholder drawing code remains intentionally temporary and private to this directory until the reusable renderer lands.
Type: runtime host implementation.
