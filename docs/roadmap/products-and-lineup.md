# Products And Lineup

This document groups the intended ScreenSave lineup.
Implemented saver products now use the canonical post-`S15` names defined in [rename-map.md](./rename-map.md).
That same document records the legacy-to-canonical migration aliases introduced during `C01`.
Future concept entries that still read like working titles are descriptive anchors, not committed final slugs. `C11` parks and routes them through `backlog-inventory.md` rather than treating them as near-term implementation promises.

## Release-Facing Reminder

- `ScreenSave Core` is the primary product for the standalone saver line.
- `anthology` remains a saver product and does not collapse into the `suite` app.
- `suite`, `benchlab`, and `sdk` remain separate companion surfaces with different roles.

## Quiet, Utility, And Ambient

- `nocturne`: low-distraction ambient saver and the first real product.
- Working title `weather_glass`: ambient utility-style saver for clocks, weather, or similar display concepts if the platform grows that far.

## Motion Family And Related Studies

- `ricochet`: polished bounce-motion saver and a real Motion Family product.
- `deepfield`: atmospheric drift and fly-through saver and a real Motion Family product.

## Framebuffer And Vector Family And Related Studies

- `plasma`: warm low-motion abstract saver and a real Framebuffer and Vector Family product. It remains the first flagship `PL` track after `SX`, not an `SX00` implementation promise.
- `phosphor`: waveform and phosphor-inspired motion and a real Framebuffer and Vector Family product.
- Working title `starfield`: classic depth and drift presentation.
- Working title `ribbon_drift`: simple motion study built around lines and trails.

## Grid And Simulation Family And Related Studies

- `pipeworks`: procedural network-growth saver and a real Grid and Simulation Family product.
- `lifeforms`: contemplative cellular-automata saver and a real Grid and Simulation Family product.
- Working title `lattice`: grid, field, and topology-driven motion.

## Systems And Ambient Family And Related Studies

- `signals`: synthetic instrument-and-panel saver and a real Systems and Ambient Family product.
- `mechanize`: restrained kinetic-assembly saver and a real Systems and Ambient Family product.
- `ecosystems`: ambient habitat-and-behavior saver and a real Systems and Ambient Family product.
- Working title `signal_garden`: procedural waveform and rule-based animation.

## Places And Atmosphere Family And Related Studies

- `stormglass`: weather-on-glass saver and a real Places and Atmosphere Family product.
- `transit`: nocturnal route-and-infrastructure saver and a real Places and Atmosphere Family product.
- `observatory`: celestial exhibit saver and a real Places and Atmosphere Family product.
- Working title `midnight_city`: atmospheric city-light presentation.
- Working title `canopy`: layered scenic motion with strong fallback discipline.
- Working title `glass_reef`: watery or reflective scenic study, only if it remains baseline-safe.

## Heavyweight Worlds Family And Related Studies

- `vector`: abstract software-3D and wireframe flyover saver and a real Heavyweight Worlds Family product.
- `explorer`: raycast-and-autopilot traversal saver and a real Heavyweight Worlds Family product.
- `city`: urban night-world scenic saver and a real Heavyweight Worlds Family product.
- `atlas`: curated fractal-voyage saver and a real Heavyweight Worlds Family product. Future Infinity Atlas ideas merge here rather than forming a separate saver.
- Working title `prism_hall`: future showcase concept only if later renderer-expansion work proves stable.

## Mathematical And Procedural

- Working title `pendulum_study`: deterministic physics-inspired motion.

## Systemic And Simulation

- Working title `cell_garden`: cellular or automata-style saver.
- Working title `particle_foundry`: longer-running particle systems with conservative fallback behavior.
- Working title `ecosystem_sketch`: simulation-oriented saver if later series justify the scope.

## Heavyweight And Showcase

- `gallery`: renderer-showcase saver for validating the current tier ladder and honest fallback behavior. It is now the bounded `SX05` and `SX06` proof surface for the real optional `gl33` modern lane and the real optional `gl46` premium lane without turning either tier into a whole-line requirement.

## Suite Meta Saver

- `anthology`: real suite meta-saver for cross-saver randomization and anthology-style playback across the other eighteen saver products. It stays separate from the real `suite` app and does not replace the individual saver `.scr` products.

## Suite-Level Products

- `suite`: real suite control app for browse, launch, preview, settings, randomization visibility, and pack visibility. `products/apps/suite/` now holds the canonical real app, and `products/apps/player/` remains only as a superseded stub.
- `benchlab`: developer-facing diagnostics harness for saver, renderer, deterministic validation, and current multi-product inspection.
- `sdk`: real contributor-facing headers, templates, examples, validation helpers, and authoring guidance for new saver products and data-driven packs.

Every product group is subordinate to the shared compatibility contract.
If a product cannot preserve the universal baseline, it must be classified explicitly rather than quietly redefining the whole suite.
