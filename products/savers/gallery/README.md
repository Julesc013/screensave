# Gallery

Gallery is the renderer-showcase saver in Series 15.

It provides:

- curated compatibility, GL11-classic, advanced-showcase, neon-abstract, technical-exhibit, quiet-museum, amber-compatibility, and GL21-prism-hall renderer tour presets
- a real saver module with product-owned config, presets, and themes
- deliberate degradation across GDI, GL11, GL21, the bounded optional GL33 modern tier, and the bounded optional GL46 premium tier while keeping the lower-tier fallback story honest
- scene families that remain coherent on the baseline while exposing richer treatment on higher renderer tiers when available
- a bounded modern-tier proof surface for `SX05` and bounded premium-tier proof surface for `SX06`, with GL33 and GL46 staying optional rather than becoming general saver-line assumptions
- tier-change-aware scenic refresh so long runs stay readable when the active renderer path changes
- preview-safe, long-run-stable behavior through the shared saver and renderer contracts
- a standalone `gallery.scr` product target with product-owned config and version resources on top of the shared host path

Gallery stays intentionally bounded.
It is not the future `suite` app, a randomizer, a scene editor, or a broad rendering lab.
