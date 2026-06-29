"""Validate the post-stable Plasma .scr default demo posture."""

from __future__ import annotations

import hashlib
import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PLASMA_SRC = ROOT / "products" / "savers" / "plasma" / "src"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "default-demo"
REPORT_JSON = REPORT_DIR / "default-demo-report.json"
REPORT_MD = REPORT_DIR / "default-demo-report.md"
ARTIFACT = ROOT / "out" / "msvc" / "vs2017_xp" / "Release" / "plasma" / "plasma.scr"
FORCED_RENDERERS = ["gdi", "gl11", "gl21", "gl33", "gl46"]
AUTO_REPORT = REPORT_DIR / "benchlab-default-auto.txt"

FORCED_REPORT_NEEDLES = [
    "Renderer status: Presented",
    "Detail level: High",
    "Randomization mode: Off",
    "Forcing active: yes",
    "Clamps: none",
    "Requested detail level: high",
    "Requested generator family: plasma",
    "Requested speed mode: gentle",
    "Requested resolution mode: fine",
    "Requested smoothing mode: soft",
    "Requested output family: raster",
    "Requested output mode: native_raster",
    "Requested filter treatment: none",
    "Requested emulation treatment: none",
    "Requested accent treatment: none",
    "Requested presentation mode: flat",
]

AUTO_REPORT_NEEDLES = [
    "Renderer status: Presented",
    "Requested renderer: Auto",
    "Policy target: OpenGL 4.6",
    "Routing profile: min GDI pref OpenGL 4.6 quality safe",
    "Requested generator family: plasma",
    "Requested output family: raster",
    "Requested output mode: native_raster",
    "Requested presentation mode: flat",
]


CHECKS = [
    {
        "id": "default_keys_preserved",
        "path": PLASMA_SRC / "plasma_internal.h",
        "needles": [
            '#define PLASMA_DEFAULT_PRESET_KEY "plasma_lava"',
            '#define PLASMA_DEFAULT_THEME_KEY "plasma_lava"',
        ],
    },
    {
        "id": "product_defaults_are_calm_native_raster",
        "path": PLASMA_SRC / "plasma_config.c",
        "needles": [
            "config->effect_mode = PLASMA_EFFECT_PLASMA;",
            "config->speed_mode = PLASMA_SPEED_GENTLE;",
            "config->resolution_mode = PLASMA_RESOLUTION_FINE;",
            "config->smoothing_mode = PLASMA_SMOOTHING_SOFT;",
            "config->output_family = PLASMA_OUTPUT_FAMILY_RASTER;",
            "config->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;",
            "config->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;",
        ],
    },
    {
        "id": "default_preset_resolves_high_fine",
        "path": PLASMA_SRC / "plasma_presets.c",
        "needles": [
            '"plasma_lava"',
            "SCREENSAVE_DETAIL_LEVEL_HIGH",
            "PLASMA_EFFECT_PLASMA",
            "PLASMA_SPEED_GENTLE",
            "PLASMA_RESOLUTION_FINE",
            "PLASMA_OUTPUT_MODE_NATIVE_RASTER",
            "PLASMA_PRESENTATION_MODE_FLAT",
        ],
    },
    {
        "id": "default_content_is_pure_classic_plasma",
        "path": PLASMA_SRC / "plasma_content.c",
        "needles": [
            '"plasma_lava"',
            "PLASMA_PRESET_MORPH_CLASS_PLASMA",
            "PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC",
            "0,\n        0,\n        0,\n        NULL",
        ],
    },
    {
        "id": "plan_defaults_match_demo",
        "path": PLASMA_SRC / "plasma_plan.c",
        "needles": [
            "plan->requested_detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;",
            "plan->requested_effect_mode = PLASMA_EFFECT_PLASMA;",
            "plan->requested_resolution_mode = PLASMA_RESOLUTION_FINE;",
            "plan->detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;",
            "plan->effect_mode = PLASMA_EFFECT_PLASMA;",
            "plan->resolution_mode = PLASMA_RESOLUTION_FINE;",
            "plan->minimum_kind = SCREENSAVE_RENDERER_KIND_GDI;",
            "plan->preferred_kind = SCREENSAVE_RENDERER_KIND_GL46;",
        ],
    },
    {
        "id": "settings_remain_user_customizable",
        "path": PLASMA_SRC / "plasma_settings.c",
        "needles": [
            '"detail_level"',
            '"low|standard|high"',
            '"effect_mode"',
            '"plasma|fire|interference|chemical_cellular_growth|lattice_quasi_crystal|caustic_marbling|aurora_curtain_ribbon|substrate_vein_coral|arc_discharge"',
            '"resolution_mode"',
            '"coarse|standard|fine"',
            '"output_family"',
            '"raster|banded|contour|glyph"',
            '"presentation_mode"',
        ],
    },
    {
        "id": "adaptive_high_resolution_field_path",
        "path": PLASMA_SRC / "plasma_sim.c",
        "needles": [
            "plasma_field_size_is_safe",
            "PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_WIDTH 1280",
            "PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_HEIGHT 720",
            "PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_WIDTH 1600",
            "PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_HEIGHT 900",
            "PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_WIDTH 1920",
            "PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_HEIGHT 1080",
            "PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_WIDTH 2560",
            "PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_HEIGHT 1440",
            "PLASMA_DEFAULT_DEMO_MIN_FIELD_WIDTH 320",
            "PLASMA_DEFAULT_DEMO_MIN_FIELD_HEIGHT 180",
            "PLASMA_DEFAULT_DEMO_DOMAIN_SIZE 4096",
            "PLASMA_DEFAULT_DEMO_DOMAIN_MAX 12288",
            "PLASMA_DEFAULT_DEMO_EDGE_GUARD 256",
            "plasma_default_demo_path",
            "plasma_compute_default_demo_field_size",
            "plasma_demo_domain_size",
            "plasma_demo_normalize_coord",
            "PLASMA_DEFAULT_DEMO_EDGE_GUARD +",
            "plasma_demo_morph_context_build",
            "plasma_classic_demo_equation_value",
            "state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL11",
            "state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33",
            "plan->resolution_mode == PLASMA_RESOLUTION_FINE",
            "plan->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH",
            "plan->output_mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER",
            "plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT",
        ],
    },
    {
        "id": "stale_registry_state_migrates_to_default_demo",
        "path": PLASMA_SRC / "plasma_config.c",
        "needles": [
            "PLASMA_DEFAULT_DEMO_SCHEMA_VERSION 4UL",
            "PLASMA_DEFAULT_DEMO_SEED 0x504C5632UL",
            "DefaultDemoSchemaVersion",
            "plasma_config_set_common_first_principles_defaults",
            "plasma_config_apply_first_principles_default",
            "plasma_config_apply_saved_preset_key",
            "plasma_config_apply_default_demo_schema",
            "common_config->use_deterministic_seed = 1;",
            "common_config->deterministic_seed = PLASMA_DEFAULT_DEMO_SEED;",
            "common_config->randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_OFF;",
            "common_config->randomization_scope = 0UL;",
            "common_config->preset_key = PLASMA_DEFAULT_PRESET_KEY;",
            "common_config->theme_key = PLASMA_DEFAULT_THEME_KEY;",
            "if (lstrcmpiA(preset_key, PLASMA_DEFAULT_PRESET_KEY) == 0)",
            "plasma_config_apply_saved_preset_key(preset_key, common_config, config);",
        ],
        "forbidden": [
            "plasma_apply_preset_bundle_to_config(PLASMA_DEFAULT_PRESET_KEY, common_config, config);",
            "if (plasma_read_string(key, \"PresetKey\", preset_key, sizeof(preset_key))) {\n        plasma_apply_preset_bundle_to_config(preset_key, common_config, config);",
        ],
    },
    {
        "id": "host_requests_display_refresh_pacing",
        "path": ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_internal.h",
        "needles": [
            "#define SCR_TIMER_FALLBACK_INTERVAL_MS 16",
            "#define SCR_TIMER_MIN_INTERVAL_MS 1",
            "#define SCR_TIMER_MAX_INTERVAL_MS 33",
        ],
    },
    {
        "id": "benchlab_requests_display_refresh_pacing",
        "path": ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_internal.h",
        "needles": [
            "#define BENCHLAB_TIMER_FALLBACK_INTERVAL_MS 16",
            "#define BENCHLAB_TIMER_MIN_INTERVAL_MS 1",
            "#define BENCHLAB_TIMER_MAX_INTERVAL_MS 33",
            "#define BENCHLAB_STEP_FALLBACK_DELTA_MS 16UL",
        ],
    },
    {
        "id": "host_uses_display_refresh_timer_request",
        "path": ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c",
        "needles": [
            "GetDeviceCaps(dc, VREFRESH)",
            "scr_refresh_to_timer_interval_ms",
            "scr_start_or_restart_timer",
            "context->timer_interval_ms = scr_resolve_timer_interval_ms(window);",
        ],
    },
    {
        "id": "benchlab_uses_display_refresh_timer_request",
        "path": ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_app.c",
        "needles": [
            "GetDeviceCaps(dc, VREFRESH)",
            "benchlab_refresh_to_timer_interval_ms",
            "app->timer_interval_ms = benchlab_resolve_timer_interval_ms(window);",
        ],
    },
    {
        "id": "slow_classic_demo_field_cycling",
        "path": PLASMA_SRC / "plasma_sim.c",
        "needles": [
            "plasma_classic_demo_equation_value",
            "plasma_smoothstep_u8",
            "plasma_demo_wave_value",
            "plasma_demo_morph_context_build",
            "broad_x = x / 20;",
            "broad_distance = distance / 20;",
            "profile->warp_amount = 6U + (plasma_demo_hash_u8(state, cycle_index, 31UL) % 24U);",
            "profile->complexity_amount = 4U + (plasma_demo_hash_u8(state, cycle_index, 43UL) % 28U);",
            "cycle_index = (state->phase_millis / 512UL) / 256UL;",
            "family_position = state->phase_millis / 512UL;",
            "value = plasma_classic_demo_equation_value(",
            "plasma_mix_u8",
            "plan->effect_mode == PLASMA_EFFECT_PLASMA",
            "speed = 2UL;",
            "return 26000UL;",
            "state->palette_phase = (state->palette_phase + ((delta_millis * speed_units) / 96UL)) & 255UL;",
        ],
        "forbidden": [
            "plasma_classic_demo_field_value",
            "lava_value",
            "morph_value",
            "value = plasma_mix_u8(lava_value, morph_value",
        ],
    },
    {
        "id": "full_color_procedural_palette_cycling",
        "path": PLASMA_SRC / "plasma_treatment.c",
        "needles": [
            "plasma_treatment_triangle_wave",
            "plasma_treatment_smoothstep_u8",
            "plasma_treatment_default_classic_path",
            "plasma_treatment_palette_context_build",
            "plasma_treatment_palette_color_from_context",
            "plasma_treatment_default_classic_path(plan, state, output)",
            "color = plasma_treatment_palette_color_from_context(&palette_context, value);",
            "color_position = state->phase_millis / 640UL;",
            "cycle_primary = plasma_treatment_morph_color(cycle_primary, target_primary, color_amount);",
            "context->primary_color = screensave_color_lerp(context->primary_color, cycle_primary, 88U);",
            "context->accent_color = screensave_color_lerp(context->accent_color, cycle_accent, 104U);",
            "context->palette_phase = (unsigned int)((state->phase_millis / 768UL) & 255UL);",
        ],
        "forbidden": [
            "plasma_treatment_classic_demo_color",
        ],
    },
    {
        "id": "renderer_ladder_exposed",
        "path": PLASMA_SRC / "plasma_module.c",
        "needles": [
            "SCREENSAVE_SAVER_CAP_GDI",
            "SCREENSAVE_SAVER_CAP_GL11",
            "SCREENSAVE_SAVER_CAP_GL21",
            "SCREENSAVE_SAVER_CAP_GL33",
            "SCREENSAVE_SAVER_CAP_GL46",
            "{ SCREENSAVE_RENDERER_KIND_GDI, SCREENSAVE_RENDERER_KIND_GL46, SCREENSAVE_CAPABILITY_QUALITY_SAFE }",
        ],
    },
    {
        "id": "auto_policy_uses_highest_supported_renderer",
        "path": ROOT / "platform" / "src" / "core" / "substrate" / "routing_policy.c",
        "needles": [
            "screensave_routing_highest_supported_kind",
            "effective_kind = highest_kind;",
            "policy_reason = screensave_routing_auto_reason(highest_kind);",
            "Routing policy applied the highest supported renderer lane.",
        ],
    },
    {
        "id": "gl33_bitmap_blit_avoids_hot_loop_texture_allocation",
        "path": ROOT / "platform" / "src" / "render" / "gl33" / "gl33_primitives.c",
        "needles": [
            "state->bitmap_texture",
            "screensave_gl33_prepare_bitmap_texture",
            "screensave_gl33_prepare_bitmap_upload_buffer",
            "screensave_gl33_release_bitmap_cache",
            "glTexSubImage2D",
            "GL_UNPACK_ROW_LENGTH",
            "upload_format = GL_BGRA;",
        ],
    },
    {
        "id": "gl46_bitmap_blit_avoids_hot_loop_texture_allocation",
        "path": ROOT / "platform" / "src" / "render" / "gl46" / "gl46_primitives.c",
        "needles": [
            "state->bitmap_texture",
            "screensave_gl46_prepare_bitmap_texture",
            "screensave_gl46_prepare_bitmap_upload_buffer",
            "screensave_gl46_release_bitmap_cache",
            "glTexSubImage2D",
            "GL_UNPACK_ROW_LENGTH",
            "upload_format = GL_BGRA;",
        ],
    },
    {
        "id": "renderer_validation_keeps_optional_tiers",
        "path": PLASMA_SRC / "plasma_validate.c",
        "needles": [
            "SCREENSAVE_RENDERER_KIND_GDI",
            "SCREENSAVE_RENDERER_KIND_GL11",
            "SCREENSAVE_RENDERER_KIND_GL21",
            "SCREENSAVE_RENDERER_KIND_GL33",
            "SCREENSAVE_RENDERER_KIND_GL46",
        ],
    },
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def sha256(path: pathlib.Path) -> str | None:
    if not path.exists():
        return None
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def run_checks(errors: list[str]) -> list[dict[str, Any]]:
    results: list[dict[str, Any]] = []
    for check in CHECKS:
        path = check["path"]
        missing: list[str] = []
        forbidden_found: list[str] = []
        if not path.exists():
            errors.append(f"Missing source for {check['id']}: {repo_path(path)}")
            results.append({"id": check["id"], "status": "fail", "path": repo_path(path), "missing": ["file"]})
            continue
        text = path.read_text(encoding="utf-8")
        for needle in check["needles"]:
            if needle not in text:
                missing.append(needle)
        for forbidden in check.get("forbidden", []):
            if forbidden in text:
                forbidden_found.append(forbidden)
        require(not missing, f"{check['id']} missing required source tokens: {missing}", errors)
        require(
            not forbidden_found,
            f"{check['id']} contains forbidden source tokens: {forbidden_found}",
            errors,
        )
        results.append({
            "id": check["id"],
            "status": "pass" if not missing and not forbidden_found else "fail",
            "path": repo_path(path),
            "missing": missing,
            "forbidden_found": forbidden_found,
        })
    return results


def run_renderer_smoke_checks(errors: list[str]) -> list[dict[str, Any]]:
    results: list[dict[str, Any]] = []
    auto_missing: list[str] = []
    if not AUTO_REPORT.exists():
        errors.append(f"missing auto renderer report: {repo_path(AUTO_REPORT)}")
    else:
        auto_text = AUTO_REPORT.read_text(encoding="utf-8")
        for needle in AUTO_REPORT_NEEDLES:
            if needle not in auto_text:
                auto_missing.append(needle)
        require(not auto_missing, f"auto renderer report missing required facts: {auto_missing}", errors)
        results.append({
            "renderer": "auto",
            "status": "pass" if not auto_missing else "fail",
            "path": repo_path(AUTO_REPORT),
            "missing": auto_missing,
        })
    for renderer in FORCED_RENDERERS:
        report_path = REPORT_DIR / f"benchlab-default-forced-{renderer}.txt"
        missing: list[str] = []
        if not report_path.exists():
            errors.append(f"Missing forced BenchLab renderer report: {repo_path(report_path)}")
            results.append({
                "renderer": renderer,
                "status": "fail",
                "path": repo_path(report_path),
                "missing": ["file"],
            })
            continue
        text = report_path.read_text(encoding="utf-8")
        for needle in FORCED_REPORT_NEEDLES:
            if needle not in text:
                missing.append(needle)
        if renderer == "gdi":
            if "Active renderer: GDI" not in text:
                missing.append("Active renderer: GDI")
        else:
            label = f"Active renderer: OpenGL {renderer[2]}.{renderer[3]}"
            if label not in text:
                missing.append(label)
        require(not missing, f"forced renderer report {renderer} missing required facts: {missing}", errors)
        results.append({
            "renderer": renderer,
            "status": "pass" if not missing else "fail",
            "path": repo_path(report_path),
            "missing": missing,
        })
    return results


def build_report(errors: list[str], checks: list[dict[str, Any]]) -> dict[str, Any]:
    artifact_hash = sha256(ARTIFACT)
    renderer_smoke = run_renderer_smoke_checks(errors)
    return {
        "schema": "screensave.plasma.default-demo.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "demo_profile": {
            "default_preset": "plasma_lava",
            "default_theme": "plasma_lava",
            "field": "single adaptive fine plasma field capped for 4K-aware software synthesis with broad-period virtual-desktop coordinates and slow equation morphing",
            "motion": "slow gentle phase motion tuned for continuous display-refresh-facing flow without tight repeating ripple periods",
            "color": "slow full-color palette-material morphing from the scalar plasma field",
            "output": "native drawable raster presentation with pixel-sharp renderer scaling and resolution-independent plasma coordinates",
            "presentation": "flat",
            "seed": "first-principles deterministic default seed 0x504C5632 with session randomization off",
            "minimum_renderer": "gdi",
            "preferred_renderer": "gl46",
            "optional_renderer_ladder": ["gl21", "gl33", "gl46"],
            "pacing": "host and BenchLab request the active display refresh interval with a 16ms fallback and 1ms minimum",
            "forced_renderer_smoke": FORCED_RENDERERS,
            "customization_boundary": "All existing Plasma settings remain explicit config inputs; presets remain starting points.",
        },
        "future_work": [
            "Capture and visually review the rebuilt .scr on target Windows hardware.",
            "Add fullscreen frame-time telemetry and contact-sheet captures; BenchLab text smoke does not replace human visual acceptance.",
            "Add per-monitor timing windows for heterogeneous multi-monitor refresh rates; the current host is one virtual-desktop window with one timer request.",
            "Refresh publication manifests only after the project chooses to replace the staged artifact.",
            "Add renderer-tier visual comparison captures for GL11/GL21/GL33 where local hardware exposes them.",
            "Add shader/native-field implementations for GL33/GL46 only after the software reference path stays visually smooth at the declared caps.",
            "Expose the new default posture through future Workbench direct-control UI without making Workbench runtime authority.",
        ],
        "artifact": {
            "path": repo_path(ARTIFACT),
            "exists": ARTIFACT.exists(),
            "sha256": artifact_hash,
        },
        "checks": checks,
        "renderer_smoke": renderer_smoke,
        "validation_errors": errors,
        "claim_boundary": (
            "Post-stable local demo rebuild evidence only; this does not publish a release, "
            "broaden compatibility certification, or replace Plasma v2 proof/publication gates."
        ),
    }


def report_markdown(report: dict[str, Any]) -> str:
    demo = report.get("demo_profile", {})
    artifact = report.get("artifact", {})
    lines = [
        "# Plasma Default Demo Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Default: {demo.get('default_preset')} + {demo.get('default_theme')}",
        f"- Field: {demo.get('field')}",
        f"- Motion: {demo.get('motion')}",
        f"- Color: {demo.get('color')}",
        f"- Output: {demo.get('output')}",
        f"- Renderers: {demo.get('minimum_renderer')} floor, {demo.get('preferred_renderer')} preferred, optional {', '.join(demo.get('optional_renderer_ladder', []))}",
        f"- Artifact: {artifact.get('path')}",
        f"- Artifact SHA256: {artifact.get('sha256') or 'missing'}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Checks",
        "",
    ]
    for check in report.get("checks", []):
        lines.append(f"- {check.get('id')}: {check.get('status')}")
    lines.extend(["", "## Renderer Smoke", ""])
    for smoke in report.get("renderer_smoke", []):
        lines.append(f"- {smoke.get('renderer')}: {smoke.get('status')} ({smoke.get('path')})")
    lines.extend(["", "## Errors", ""])
    errors = report.get("validation_errors", [])
    if errors:
        for error in errors:
            lines.append(f"- {error}")
    else:
        lines.append("- none")
    lines.extend(["", "## Future Work", ""])
    for item in report.get("future_work", []):
        lines.append(f"- {item}")
    lines.append("")
    return "\n".join(lines)


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    REPORT_MD.write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    errors: list[str] = []
    checks = run_checks(errors)
    report = build_report(errors, checks)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma default demo checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
