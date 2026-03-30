"""Validate the C13 Wave C saver-polish surface and active status docs."""

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

    wave_note_path = ROOT / "validation" / "notes" / "c13-wave-c-polish.md"
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
        ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_presets.c",
        ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_themes.c",
        ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_module.c",
        ROOT / "products" / "savers" / "transit" / "src" / "transit_presets.c",
        ROOT / "products" / "savers" / "transit" / "src" / "transit_themes.c",
        ROOT / "products" / "savers" / "observatory" / "src" / "observatory_presets.c",
        ROOT / "products" / "savers" / "observatory" / "src" / "observatory_themes.c",
        ROOT / "products" / "savers" / "observatory" / "src" / "observatory_module.c",
        ROOT / "products" / "savers" / "vector" / "src" / "vector_presets.c",
        ROOT / "products" / "savers" / "vector" / "src" / "vector_themes.c",
        ROOT / "products" / "savers" / "explorer" / "src" / "explorer_presets.c",
        ROOT / "products" / "savers" / "explorer" / "src" / "explorer_themes.c",
        ROOT / "products" / "savers" / "explorer" / "src" / "explorer_module.c",
        ROOT / "products" / "savers" / "city" / "src" / "city_presets.c",
        ROOT / "products" / "savers" / "city" / "src" / "city_themes.c",
        ROOT / "products" / "savers" / "city" / "src" / "city_module.c",
        ROOT / "products" / "savers" / "atlas" / "src" / "atlas_presets.c",
        ROOT / "products" / "savers" / "atlas" / "src" / "atlas_themes.c",
        ROOT / "products" / "savers" / "gallery" / "src" / "gallery_presets.c",
        ROOT / "products" / "savers" / "gallery" / "src" / "gallery_themes.c",
        ROOT / "products" / "savers" / "gallery" / "src" / "gallery_module.c",
        ROOT / "products" / "savers" / "anthology" / "src" / "anthology_presets.c",
        ROOT / "products" / "savers" / "anthology" / "src" / "anthology_themes.c",
        ROOT / "products" / "savers" / "anthology" / "src" / "anthology_sim.c",
    )

    for path in required_paths:
        require(path.exists(), f"Missing required C13 Wave C path: {path.relative_to(ROOT)}", errors)

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

    stormglass_presets = read_text(ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_presets.c")
    stormglass_themes = read_text(ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_themes.c")
    stormglass_module = read_text(ROOT / "products" / "savers" / "stormglass" / "src" / "stormglass_module.c")
    transit_presets = read_text(ROOT / "products" / "savers" / "transit" / "src" / "transit_presets.c")
    transit_themes = read_text(ROOT / "products" / "savers" / "transit" / "src" / "transit_themes.c")
    observatory_presets = read_text(ROOT / "products" / "savers" / "observatory" / "src" / "observatory_presets.c")
    observatory_themes = read_text(ROOT / "products" / "savers" / "observatory" / "src" / "observatory_themes.c")
    observatory_module = read_text(ROOT / "products" / "savers" / "observatory" / "src" / "observatory_module.c")
    vector_presets = read_text(ROOT / "products" / "savers" / "vector" / "src" / "vector_presets.c")
    vector_themes = read_text(ROOT / "products" / "savers" / "vector" / "src" / "vector_themes.c")
    explorer_presets = read_text(ROOT / "products" / "savers" / "explorer" / "src" / "explorer_presets.c")
    explorer_themes = read_text(ROOT / "products" / "savers" / "explorer" / "src" / "explorer_themes.c")
    explorer_module = read_text(ROOT / "products" / "savers" / "explorer" / "src" / "explorer_module.c")
    city_presets = read_text(ROOT / "products" / "savers" / "city" / "src" / "city_presets.c")
    city_themes = read_text(ROOT / "products" / "savers" / "city" / "src" / "city_themes.c")
    city_module = read_text(ROOT / "products" / "savers" / "city" / "src" / "city_module.c")
    atlas_presets = read_text(ROOT / "products" / "savers" / "atlas" / "src" / "atlas_presets.c")
    atlas_themes = read_text(ROOT / "products" / "savers" / "atlas" / "src" / "atlas_themes.c")
    gallery_presets = read_text(ROOT / "products" / "savers" / "gallery" / "src" / "gallery_presets.c")
    gallery_themes = read_text(ROOT / "products" / "savers" / "gallery" / "src" / "gallery_themes.c")
    gallery_module = read_text(ROOT / "products" / "savers" / "gallery" / "src" / "gallery_module.c")
    anthology_presets = read_text(ROOT / "products" / "savers" / "anthology" / "src" / "anthology_presets.c")
    anthology_themes = read_text(ROOT / "products" / "savers" / "anthology" / "src" / "anthology_themes.c")
    anthology_sim = read_text(ROOT / "products" / "savers" / "anthology" / "src" / "anthology_sim.c")

    for snippet in (
        "# C13 Wave C Polish Note",
        "- `stormglass`",
        "- `transit`",
        "- `observatory`",
        "- `vector`",
        "- `explorer`",
        "- `city`",
        "- `atlas`",
        "- `gallery`",
        "- `anthology`",
        "- `C14` final rerelease hardening comes next.",
    ):
        require(snippet in wave_note, f"c13-wave-c-polish.md is missing {snippet!r}.", errors)

    require("winter_streetlamp" in stormglass_presets, "Stormglass Wave C preset surface is missing 'winter_streetlamp'.", errors)
    require("monochrome_cold_pane" in stormglass_themes, "Stormglass Wave C theme surface is missing 'monochrome_cold_pane'.", errors)
    require("stormglass_config_randomize_settings" in stormglass_module, "Stormglass module must wire randomize_settings.", errors)
    require("motorway_fog" in transit_presets, "Transit Wave C preset surface is missing 'motorway_fog'.", errors)
    require("harbor_midnight" in transit_themes, "Transit Wave C theme surface is missing 'harbor_midnight'.", errors)
    require("planetarium_blue" in observatory_presets, "Observatory Wave C preset surface is missing 'planetarium_blue'.", errors)
    require("brass_eclipse" in observatory_themes, "Observatory Wave C theme surface is missing 'brass_eclipse'.", errors)
    require("observatory_config_randomize_settings" in observatory_module, "Observatory module must wire randomize_settings.", errors)
    require("amber_wire_tunnel" in vector_presets, "Vector Wave C preset surface is missing 'amber_wire_tunnel'.", errors)
    require("terrain_museum_glide" in vector_themes, "Vector Wave C theme surface is missing 'terrain_museum_glide'.", errors)
    require("eerie_corridor_watch" in explorer_presets, "Explorer Wave C preset surface is missing 'eerie_corridor_watch'.", errors)
    require("amber_service_drift" in explorer_themes, "Explorer Wave C theme surface is missing 'amber_service_drift'.", errors)
    require("explorer_config_randomize_settings" in explorer_module, "Explorer module must wire randomize_settings.", errors)
    require("blue_harbor_watch" in city_presets, "City Wave C preset surface is missing 'blue_harbor_watch'.", errors)
    require("neon_rooftop_drift" in city_themes, "City Wave C theme surface is missing 'neon_rooftop_drift'.", errors)
    require("city_config_randomize_settings" in city_module, "City module must wire randomize_settings.", errors)
    require("nebula_voyage" in atlas_presets, "Atlas Wave C preset surface is missing 'nebula_voyage'.", errors)
    require("mathematics_plate" in atlas_themes, "Atlas Wave C theme surface is missing 'mathematics_plate'.", errors)
    require("amber_compatibility" in gallery_presets, "Gallery Wave C preset surface is missing 'amber_compatibility'.", errors)
    require("gl21_prism_hall" in gallery_themes, "Gallery Wave C theme surface is missing 'gl21_prism_hall'.", errors)
    require("gallery_config_randomize_settings" in gallery_module, "Gallery module must wire randomize_settings.", errors)
    require("scenic_grand_tour" in anthology_presets, "Anthology Wave C preset surface is missing 'scenic_grand_tour'.", errors)
    require("anthology_midnight" in anthology_themes, "Anthology Wave C theme surface is missing 'anthology_midnight'.", errors)
    require("45000UL" in anthology_sim, "Anthology sim must cap preview interval millis.", errors)
    require("previous_catalog_index" in anthology_sim and "/ 2UL" in anthology_sim, "Anthology sim must damp direct repeat loops.", errors)

    require(
        "Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, and `C14`." in readme,
        "README.md must record C13 Wave C as complete.",
        errors,
    )
    require(
        "The current continuation line is complete through `C14` final rerelease hardening." in readme,
        "README.md must point to C14.",
        errors,
    )
    require(
        "`C13` Wave A, Wave B, and Wave C are complete, and `C14` final rerelease hardening has closed the current continuation line with the release-candidate pass." in prompt_program,
        "prompt-program.md must point to C14.",
        errors,
    )
    require(
        "`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, and `C14` are complete." in series_map,
        "series-map.md must record C13 Wave C as complete.",
        errors,
    )
    require(
        "The current continuation line is complete through `C14` final rerelease hardening." in series_map,
        "series-map.md must point to C14.",
        errors,
    )
    require(
        "No further continuation prompt is scheduled on the current line after `C14` final rerelease hardening." in post_s15,
        "post-s15-plan.md must point to C14.",
        errors,
    )
    require("## C13 Wave C - 2026-03-30" in changelog, "CHANGELOG.md must record C13 Wave C.", errors)

    for snippet in (
        "`Wave-C` landed for `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` while keeping `suite` and `benchlab` limited to narrow validation support.",
        "`C14` final rerelease hardening is complete; later work is post-release follow-on rather than another scheduled continuation prompt.",
    ):
        require(snippet in backlog_inventory, f"backlog-inventory.md is missing {snippet!r}.", errors)

    for snippet in (
        "`Wave-C` handled `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` without reopening suite-wide vocabulary or the shared platform contract.",
        "`C14` final rerelease hardening is complete; later work is post-release follow-on rather than another scheduled continuation prompt.",
    ):
        require(snippet in backlog_routing, f"backlog-routing.md is missing {snippet!r}.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Wave C surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
