# Plasma Default Demo Report

- Status: pass
- Default: plasma_lava + plasma_lava
- Field: adaptive high-resolution fine plasma field capped by renderer tier with classic layered wave synthesis
- Motion: slower gentle phase motion tuned for display-refresh-facing flow
- Color: direct full-color smooth RGB palette cycling from the scalar plasma field
- Output: native drawable raster presentation with pixel-sharp renderer scaling and resolution-independent plasma coordinates
- Renderers: gdi floor, gl46 preferred, optional gl21, gl33, gl46
- Artifact: out/msvc/vs2017_xp/Release/plasma/plasma.scr
- Artifact SHA256: df83baec3bf76a41d8922407172efffc0603bc3aff6265fe4e45eef40bb7e089
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
- Add shader/native-field implementations for GL33/GL46 if CPU field synthesis remains too expensive at native high-refresh targets.
- Expose the new default posture through future Workbench direct-control UI without making Workbench runtime authority.
