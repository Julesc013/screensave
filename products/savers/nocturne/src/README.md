# Nocturne Source

Purpose: product-local implementation for the Nocturne saver.
Belongs here: module lifecycle, config dialog and persistence, presets, themes, simulation, and rendering owned by Nocturne.
Does not belong here: shared platform code or logic that only exists to prepare future savers.

Key files:

- `nocturne_module.c` declares the real saver module descriptor
- `nocturne_config.c` owns Nocturne defaults, persistence, and its config dialog
- `nocturne_presets.c` and `nocturne_themes.c` define the curated product identity
- `nocturne_sim.c` owns fade, reseed, and motion-state evolution
- `nocturne_render.c` draws the restrained visual through the shared renderer contract

Type: product runtime source.
