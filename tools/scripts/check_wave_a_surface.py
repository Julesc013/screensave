"""Validate the C13 Wave A saver-polish surface and active status docs."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    wave_note_path = ROOT / "validation" / "notes" / "c13-wave-a-polish.md"
    backlog_inventory_path = ROOT / "docs" / "roadmap" / "backlog-inventory.md"
    backlog_routing_path = ROOT / "docs" / "roadmap" / "backlog-routing.md"
    readme_path = ROOT / "README.md"
    prompt_program_path = ROOT / "docs" / "roadmap" / "prompt-program.md"
    series_map_path = ROOT / "docs" / "roadmap" / "series-map.md"
    post_s15_path = ROOT / "docs" / "roadmap" / "post-s15-plan.md"
    changelog_path = ROOT / "CHANGELOG.md"

    required_paths = (
        wave_note_path,
        backlog_inventory_path,
        backlog_routing_path,
        readme_path,
        prompt_program_path,
        series_map_path,
        post_s15_path,
        changelog_path,
        ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_presets.c",
        ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_themes.c",
        ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_presets.c",
        ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_module.c",
        ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_presets.c",
        ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_themes.c",
        ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_module.c",
        ROOT / "products" / "savers" / "plasma" / "src" / "plasma_presets.c",
        ROOT / "products" / "savers" / "plasma" / "src" / "plasma_themes.c",
        ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_presets.c",
        ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_themes.c",
        ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_module.c",
    )

    for path in required_paths:
        require(path.exists(), f"Missing required C13 Wave A path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    wave_note = read_text(wave_note_path)
    backlog_inventory = read_text(backlog_inventory_path)
    backlog_routing = read_text(backlog_routing_path)
    readme = read_text(readme_path)
    prompt_program = read_text(prompt_program_path)
    series_map = read_text(series_map_path)
    post_s15 = read_text(post_s15_path)
    changelog = read_text(changelog_path)

    nocturne_presets = read_text(ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_presets.c")
    nocturne_themes = read_text(ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_themes.c")
    ricochet_presets = read_text(ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_presets.c")
    ricochet_module = read_text(ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_module.c")
    deepfield_presets = read_text(ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_presets.c")
    deepfield_themes = read_text(ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_themes.c")
    deepfield_module = read_text(ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_module.c")
    plasma_presets = read_text(ROOT / "products" / "savers" / "plasma" / "src" / "plasma_presets.c")
    plasma_themes = read_text(ROOT / "products" / "savers" / "plasma" / "src" / "plasma_themes.c")
    phosphor_presets = read_text(ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_presets.c")
    phosphor_themes = read_text(ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_themes.c")
    phosphor_module = read_text(ROOT / "products" / "savers" / "phosphor" / "src" / "phosphor_module.c")

    for snippet in (
        "# C13 Wave A Polish Note",
        "- `nocturne`",
        "- `ricochet`",
        "- `deepfield`",
        "- `plasma`",
        "- `phosphor`",
        "- `C13` Wave B has now landed for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`",
        "- `C13` Wave C for the final saver-specific polish wave",
    ):
        require(snippet in wave_note, f"c13-wave-a-polish.md is missing {snippet!r}.", errors)

    require("phosphor_night" in nocturne_presets, "Nocturne Wave A preset surface is missing 'phosphor_night'.", errors)
    require("museum_amber" in nocturne_themes, "Nocturne Wave A theme surface is missing 'museum_amber'.", errors)
    require("amber_arc" in ricochet_presets, "Ricochet Wave A preset surface is missing 'amber_arc'.", errors)
    require("quiet_corporate" in ricochet_presets, "Ricochet Wave A preset surface is missing 'quiet_corporate'.", errors)
    require("ricochet_config_randomize_settings" in ricochet_module, "Ricochet module must wire randomize_settings.", errors)
    require("deep_midnight" in deepfield_presets, "Deepfield Wave A preset surface is missing 'deep_midnight'.", errors)
    require("deep_midnight" in deepfield_themes, "Deepfield Wave A theme surface is missing 'deep_midnight'.", errors)
    require("deepfield_config_randomize_settings" in deepfield_module, "Deepfield module must wire randomize_settings.", errors)
    require("midnight_interference" in plasma_presets, "Plasma Wave A preset surface is missing 'midnight_interference'.", errors)
    require("amber_terminal" in plasma_themes, "Plasma Wave A theme surface is missing 'amber_terminal'.", errors)
    require("amber_harmonics" in phosphor_presets, "Phosphor Wave A preset surface is missing 'amber_harmonics'.", errors)
    require("white_instrument" in phosphor_themes, "Phosphor Wave A theme surface is missing 'white_instrument'.", errors)
    require("phosphor_config_randomize_settings" in phosphor_module, "Phosphor module must wire randomize_settings.", errors)

    require(
        "Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, and `C15`." in readme,
        "README.md must record C13 Wave C as complete.",
        errors,
    )
    require(
        "The active continuation line now extends through `C15` release doctrine and channel split. `C16` Core release refresh and baseline freeze is next." in readme,
        "README.md must point to C15 and C16.",
        errors,
    )
    require(
        "`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, and `C15` release doctrine and channel split are complete. `C16` Core release refresh and baseline freeze is next." in prompt_program,
        "prompt-program.md must record the current C15/C16 status.",
        errors,
    )
    require(
        "The active continuation line now extends through `C15` release doctrine and channel split. `C16` Core release refresh and baseline freeze is next." in series_map,
        "series-map.md must point to C15 and C16.",
        errors,
    )
    require(
        "`C16` Core release refresh and baseline freeze is the next continuation prompt." in post_s15,
        "post-s15-plan.md must point to C16 as next.",
        errors,
    )
    require("## C13 Wave A - 2026-03-30" in changelog, "CHANGELOG.md must record C13 Wave A.", errors)

    for snippet in (
        "`C13` Wave A lands the first premium pass",
        "`C13` Wave A preserves `plasma` as the framebuffer anchor",
        "`C13` Wave A keeps capability claims honest while polishing phosphor identity",
    ):
        require(snippet in backlog_inventory, f"backlog-inventory.md is missing {snippet!r}.", errors)

    for snippet in (
        "`Wave-A` | First saver-specific polish wave | `nocturne`, `ricochet`, `deepfield`, `plasma`, and `phosphor`",
        "Implemented for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A",
        "`Wave-B` handled `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A",
    ):
        require(snippet in backlog_routing, f"backlog-routing.md is missing {snippet!r}.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Wave A surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
