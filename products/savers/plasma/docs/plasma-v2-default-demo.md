# Plasma v2 Default Demo

Plasma's out-of-box `.scr` default is now treated as a calm classic plasma demo
base for the v2 architecture. It should open as a high-detail, full-color,
slow-moving procedural field rather than as a preset picker or a proof-only
shell.

The default posture is:

- preset key: `plasma_lava`
- theme key: `plasma_lava`
- effect mode: `plasma`
- speed: `gentle`
- detail: `high`
- resolution: `fine`
- output: `raster` / `native_raster`
- presentation: `flat`
- required renderer floor: GDI
- preferred renderer: GL46 through auto routing, with fallback through GL33,
  GL21, GL11, and GDI
- optional exposed renderer tiers: GL21, GL33, GL46

The default `plasma_lava` content entry is intentionally not advanced, modern,
or premium capable. Platform auto routing asks for the highest advertised tier
first, then falls back through the public renderer ladder. Requesting
GL11/GL21/GL33/GL46 may accelerate or improve presentation, but it must not
automatically turn the default into a different effect stack. Extra generator
families, treatments, overlays, contours, glyphs, ribbons, and heightfields
remain explicit opt-in settings or separate authored presets.

## Runtime Behavior

The default plasma effect uses the existing product path:

```text
config / preset / settings
  -> plasma plan
  -> plasma runtime
  -> field pipeline
  -> material and treatment
  -> renderer blit
```

It does not add hidden preset authority. Presets and settings remain explicit
inputs, while runtime behavior comes from the resolved plan.

For the default `plasma` effect, the runtime now favors:

- native host drawable presentation: the `.scr` window is sized to the current
  monitor or virtual desktop surface, so 1440p, 1080p, 4K, VGA, and mixed
  monitor layouts receive a native output surface from the host
- adaptive high-resolution field sizing outside preview mode when high detail
  and fine resolution are selected. The default demo now uses tiered caps:
  1280x720 for GDI, 1920x1080 for GL11, 2560x1440 for GL21, and up to
  6144x2304 for GL33/GL46. Smaller drawable surfaces run at native field
  resolution; larger virtual desktops are scaled down as one continuous field
  instead of tiled per-monitor chunks.
- aspect-aware virtual-desktop coordinates with an edge guard. The visible
  field maps into a larger procedural domain so sharp mathematical fold
  boundaries are pushed outside the drawable or aligned to the outer virtual
  desktop edges, reducing visible seams and repeated bands across mixed
  monitor layouts.
- renderer-side presentation from the internal semantic field to the native
  drawable surface, with the same equation/material/treatment state across
  GDI, GL11, GL21, GL33, and GL46 where those lanes are available
- slower gentle phase motion tuned for display-refresh-facing flow rather than
  reading as a stream of still fields
- classic layered plasma wave synthesis: horizontal, vertical, diagonal, and
  two slowly drifting radial wave sources resolved into one scalar field
- slow equation morphing inside the default lava field. The lava equation
  remains the visual anchor, while secondary field families blend into it
  procedurally instead of cutting or fading between presets.
- no random composition jumps in the default plasma effect; non-default effects
  can still use composition refresh as explicit alternate modes
- direct smooth RGB palette cycling from the scalar plasma field, instead of
  theme-endpoint bands, contours, overlays, glyphs, or postprocessing
- restrained dark-room brightness through bounded channel ranges

Preview mode remains more conservative so the Windows preview pane does not
force a large internal field.

Existing user registry settings from earlier Plasma builds may not carry the
new default-demo schema marker. On load, those stale product settings migrate
back to the current calm first-principles default path so old experimental
output, randomization, preset-set, theme-set, journey, transition, treatment,
or presentation state cannot silently make the freshly built `.scr` look like
the previous months-old demo. Saves write `DefaultDemoSchemaVersion`, and
schema version 4 resets older saved Plasma state to:

- deterministic seed `0x504C5632`
- randomization off
- transitions off
- no preset/theme set selection
- no journey selection
- high detail, fine resolution, gentle motion
- native raster output
- flat presentation
- no sampling, filter, emulation, or accent treatment

The `plasma_lava` preset/theme keys remain as compatibility identity labels for
older APIs and the settings UI. They are not the default runtime authority, and
loading a saved default key does not reapply the legacy preset bundle on the
next run.

The classic default modulation is runtime motion inside the resolved `plasma`
effect. It does not mutate saved settings and it does not make old presets,
themes, or journeys hidden runtime masters. User-configured transition journeys
remain available as explicit settings.

## Customization

The existing settings remain the customization surface:

- detail level
- effect mode
- speed mode
- resolution mode
- smoothing mode
- output family and mode
- treatment options
- presentation mode
- preset and theme starting points

The default is only a starting point. It should be good enough to run as a
classic plasma demo immediately, while still leaving direct controls available
for Workbench and configuration UI.

## Boundaries

This default-demo rebuild is not a public release publication. It is also not a
compatibility certification broadening, not a public SDK freeze, and not a
claim that every higher renderer tier has independent visual acceptance.

GDI remains the mandatory floor. GL11 and higher renderer tiers remain optional
and capability-gated.

## Performance Posture

The current `.scr` runtime is still a CPU field synthesizer followed by a
renderer blit/present. GDI and OpenGL therefore share the same resolved Plasma
field semantics; OpenGL can help with presentation and scaling, but this repair
does not claim shader-side field computation.

The Windows saver host and BenchLab now request a timer interval from the active
display refresh (`VREFRESH`) with a 16ms fallback. A 155Hz panel therefore gets
an approximately 6ms timer request, while a 60Hz display gets an approximately
17ms timer request. This is a request, not a guarantee: the real frame cadence
still depends on Windows timer delivery, display refresh, driver behavior,
renderer initialization, and machine load. The Plasma field budget is tiered by
renderer capability. GL33/GL46 can now receive much larger field surfaces than
the earlier 720p cap, while GDI remains bounded as the compatibility floor.

GL33 and GL46 bitmap presentation now reuse a renderer-owned texture and upload
buffer instead of allocating a conversion buffer and texture every frame. The
normal 32-bit visual-buffer path uploads BGRA directly and adjusts texture
coordinates for top-left bitmap origin. This keeps the product architecture
unchanged while removing a major jitter source from the higher renderer lanes.

The adaptive field cap is the product-local foundation for broad monitor
coverage:

- VGA, HD, and FHD displays can run close to or exactly at native field
  resolution depending on renderer tier.
- 1440p displays are covered natively on GL21 and higher.
- 4K and broad mixed-monitor layouts are covered by the high GL33/GL46 field
  budget when they fit within the 6144x2304 cap; larger desktop walls remain
  one continuous scaled field rather than mirrored or repeated tiles.
- Multiple monitors are treated through the host-provided virtual desktop
  drawable size. This covers mixed monitor layouts, but the current host still
  owns one timer for one saver window. Per-monitor timing, per-monitor windows,
  explicit vsync control, and shader-side field synthesis are future platform
  work for heterogeneous 155Hz/60Hz setups and arbitrarily large monitor walls.

The next platform-level optimization should be a shared renderer capability and
frame-budget service that lets products request a semantic quality target while
the platform chooses CPU, integrated-GPU, or discrete-GPU presentation strategy.
That should remain optional and capability-gated: the GDI path stays valid, and
GL shader or texture-pipeline work must never become the only way Plasma runs.

## Future Work

- Capture the rebuilt `.scr` on target Windows hardware and review the visuals.
- Add fullscreen frame-time telemetry and contact-sheet capture. Current
  BenchLab text smoke proves direct-control routing and renderer presentation,
  not perceptual smoothness on every display.
- Design shared per-monitor frame-budget, vsync, and renderer capability
  services as separate platform gates; this repair keeps the product-local
  runtime smooth under the existing host frame contract.
- Add shader/native-field implementations for GL33/GL46 so arbitrarily large
  native-resolution monitor walls can stay high-refresh without relying on CPU
  desktop-pixel synthesis.
- Refresh release/package manifests only if the rebuilt artifact replaces the
  staged publication artifact.
- Add renderer-tier comparison captures for GL11, GL21, and GL33 where local
  hardware exposes those paths.
- Add explicit UI controls for field wavelength, radial-source drift, palette
  drift, edge guard, field cap policy, and equation morphing if the project
  wants these as named direct controls rather than just default-demo runtime
  posture.
- Surface this default posture through a future Workbench direct-control panel.
- Keep graph-style authoring, if added later, lowering into the same
  spec/plan/runtime path rather than becoming runtime authority.
