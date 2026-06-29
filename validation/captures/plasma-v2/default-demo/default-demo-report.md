# Plasma Default Demo Report

- Status: pass
- Default: plasma_lava + plasma_lava
- Field: single adaptive fine plasma field capped for 4K-aware software synthesis with broad-period virtual-desktop coordinates and slow equation morphing
- Motion: slow gentle phase motion tuned for continuous display-refresh-facing flow without tight repeating ripple periods
- Color: slow full-color palette-material morphing from the scalar plasma field
- Output: native drawable raster presentation with pixel-sharp renderer scaling and resolution-independent plasma coordinates
- Renderers: gdi floor, gl46 preferred, optional gl21, gl33, gl46
- Artifact: out/msvc/vs2017_xp/Release/plasma/plasma.scr
- Artifact SHA256: 50080152f3b8300ea2cc7242a861af648b875911626b8fc804475f3695f58f64
- Claim boundary: Post-stable local demo rebuild evidence only; this does not publish a release, broaden compatibility certification, or replace Plasma v2 proof/publication gates.

## Checks

- default_keys_preserved: pass
- product_defaults_are_calm_native_raster: pass
- default_preset_resolves_high_fine: pass
- default_content_is_pure_classic_plasma: pass
- plan_defaults_match_demo: pass
- settings_remain_user_customizable: pass
- adaptive_high_resolution_field_path: pass
- stale_registry_state_migrates_to_default_demo: pass
- host_requests_display_refresh_pacing: pass
- benchlab_requests_display_refresh_pacing: pass
- host_uses_display_refresh_timer_request: pass
- benchlab_uses_display_refresh_timer_request: pass
- slow_classic_demo_field_cycling: pass
- full_color_procedural_palette_cycling: pass
- renderer_ladder_exposed: pass
- auto_policy_uses_highest_supported_renderer: pass
- gl33_bitmap_blit_avoids_hot_loop_texture_allocation: pass
- gl46_bitmap_blit_avoids_hot_loop_texture_allocation: pass
- renderer_validation_keeps_optional_tiers: pass

## Renderer Smoke

- auto: pass (validation/captures/plasma-v2/default-demo/benchlab-default-auto.txt)
- gdi: pass (validation/captures/plasma-v2/default-demo/benchlab-default-forced-gdi.txt)
- gl11: pass (validation/captures/plasma-v2/default-demo/benchlab-default-forced-gl11.txt)
- gl21: pass (validation/captures/plasma-v2/default-demo/benchlab-default-forced-gl21.txt)
- gl33: pass (validation/captures/plasma-v2/default-demo/benchlab-default-forced-gl33.txt)
- gl46: pass (validation/captures/plasma-v2/default-demo/benchlab-default-forced-gl46.txt)

## Errors

- none

## Future Work

- Capture and visually review the rebuilt .scr on target Windows hardware.
- Add fullscreen frame-time telemetry and contact-sheet captures; BenchLab text smoke does not replace human visual acceptance.
- Add per-monitor timing windows for heterogeneous multi-monitor refresh rates; the current host is one virtual-desktop window with one timer request.
- Refresh publication manifests only after the project chooses to replace the staged artifact.
- Add renderer-tier visual comparison captures for GL11/GL21/GL33 where local hardware exposes them.
- Add shader/native-field implementations for GL33/GL46 only after the software reference path stays visually smooth at the declared caps.
- Expose the new default posture through future Workbench direct-control UI without making Workbench runtime authority.
