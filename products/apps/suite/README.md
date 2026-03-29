# Suite

Suite is the real suite-level browser, launcher, preview, and configuration surface introduced in continuation `C09`.

It provides:

- a real `suite.exe` app product under `products/apps/suite/`
- manifest-driven browsing across the current standalone saver line plus the `anthology` meta-saver
- embedded live preview, detached windowed run, and bounded saver `Settings...` handoff
- visibility into saver metadata, presets, themes, randomization state, local pack samples, and honest `.scr` artifact availability
- use of the shared saver, renderer, config, and settings contracts without replacing the standalone `.scr` products
- the shared suite vocabulary for renderer preference, session randomization, apply, and revert actions without becoming a giant editor surface

Suite is intentionally bounded.
It is not BenchLab, not the `anthology` meta-saver, not a marketplace, not a content IDE, and not the future SDK surface.
