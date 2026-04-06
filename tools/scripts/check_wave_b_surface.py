"""Validate the C13 Wave B saver-polish surface and active status docs."""

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

    wave_note_path = ROOT / "validation" / "notes" / "c13-wave-b-polish.md"
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
        ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_presets.c",
        ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_themes.c",
        ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_module.c",
        ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_presets.c",
        ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_themes.c",
        ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_module.c",
        ROOT / "products" / "savers" / "signals" / "src" / "signals_presets.c",
        ROOT / "products" / "savers" / "signals" / "src" / "signals_themes.c",
        ROOT / "products" / "savers" / "signals" / "src" / "signals_module.c",
        ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_presets.c",
        ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_themes.c",
        ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_module.c",
        ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_presets.c",
        ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_themes.c",
        ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_module.c",
    )

    for path in required_paths:
        require(path.exists(), f"Missing required C13 Wave B path: {path.relative_to(ROOT)}", errors)

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

    pipeworks_presets = read_text(ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_presets.c")
    pipeworks_themes = read_text(ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_themes.c")
    pipeworks_module = read_text(ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_module.c")
    lifeforms_presets = read_text(ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_presets.c")
    lifeforms_themes = read_text(ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_themes.c")
    lifeforms_module = read_text(ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_module.c")
    signals_presets = read_text(ROOT / "products" / "savers" / "signals" / "src" / "signals_presets.c")
    signals_themes = read_text(ROOT / "products" / "savers" / "signals" / "src" / "signals_themes.c")
    signals_module = read_text(ROOT / "products" / "savers" / "signals" / "src" / "signals_module.c")
    mechanize_presets = read_text(ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_presets.c")
    mechanize_themes = read_text(ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_themes.c")
    mechanize_module = read_text(ROOT / "products" / "savers" / "mechanize" / "src" / "mechanize_module.c")
    ecosystems_presets = read_text(ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_presets.c")
    ecosystems_themes = read_text(ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_themes.c")
    ecosystems_module = read_text(ROOT / "products" / "savers" / "ecosystems" / "src" / "ecosystems_module.c")

    for snippet in (
        "# C13 Wave B Polish Note",
        "- `pipeworks`",
        "- `lifeforms`",
        "- `signals`",
        "- `mechanize`",
        "- `ecosystems`",
        "- `C13` Wave C for the final saver-specific polish wave",
    ):
        require(snippet in wave_note, f"c13-wave-b-polish.md is missing {snippet!r}.", errors)

    require("amber_backplane" in pipeworks_presets, "Pipeworks Wave B preset surface is missing 'amber_backplane'.", errors)
    require("midnight_blueprint" in pipeworks_themes, "Pipeworks Wave B theme surface is missing 'midnight_blueprint'.", errors)
    require("pipeworks_config_randomize_settings" in pipeworks_module, "Pipeworks module must wire randomize_settings.", errors)
    require("garden_bloom" in lifeforms_presets, "Lifeforms Wave B preset surface is missing 'garden_bloom'.", errors)
    require("blue_archive" in lifeforms_themes, "Lifeforms Wave B theme surface is missing 'blue_archive'.", errors)
    require("lifeforms_config_randomize_settings" in lifeforms_module, "Lifeforms module must wire randomize_settings.", errors)
    require("telemetry_wall" in signals_presets, "Signals Wave B preset surface is missing 'telemetry_wall'.", errors)
    require("night_watch_console" in signals_themes, "Signals Wave B theme surface is missing 'night_watch_console'.", errors)
    require("signals_config_randomize_settings" in signals_module, "Signals module must wire randomize_settings.", errors)
    require("copper_counterworks" in mechanize_presets, "Mechanize Wave B preset surface is missing 'copper_counterworks'.", errors)
    require("ivory_gallery" in mechanize_themes, "Mechanize Wave B theme surface is missing 'ivory_gallery'.", errors)
    require("mechanize_config_randomize_settings" in mechanize_module, "Mechanize module must wire randomize_settings.", errors)
    require("reef_current" in ecosystems_presets, "Ecosystems Wave B preset surface is missing 'reef_current'.", errors)
    require("night_marsh" in ecosystems_themes, "Ecosystems Wave B theme surface is missing 'night_marsh'.", errors)
    require("ecosystems_config_randomize_settings" in ecosystems_module, "Ecosystems module must keep randomize_settings wired.", errors)

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
        "prompt-program.md must record C15 and point to C16.",
        errors,
    )
    require(
        "`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, and `C15` are complete." in series_map,
        "series-map.md must record C13 Wave C plus C15 as complete.",
        errors,
    )
    require(
        "The active continuation line now extends through `C15` release doctrine and channel split. `C16` Core release refresh and baseline freeze is next." in series_map,
        "series-map.md must point to C15 and C16.",
        errors,
    )
    require(
        "15. `C13` Wave B completed the second saver-specific polish pass for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`." in post_s15,
        "post-s15-plan.md must record C13 Wave B as complete.",
        errors,
    )
    require(
        "`C16` Core release refresh and baseline freeze is the next continuation prompt." in post_s15,
        "post-s15-plan.md must point to C16 as next.",
        errors,
    )
    require("## C13 Wave B - 2026-03-30" in changelog, "CHANGELOG.md must record C13 Wave B.", errors)

    for snippet in (
        "`C13` Wave B keeps grid-growth polish product-local",
        "`C13` Wave B improves contemplative long-run behavior",
        "`C13` Wave B keeps system-interface polish product-local",
        "`C13` Wave B avoids promoting machine-specific logic",
        "`C13` Wave B keeps habitat behavior product-owned",
        "`Wave-B` covers `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`.",
        "`Wave-C` landed for `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` while keeping `suite` and `benchlab` limited to narrow validation support.",
    ):
        require(snippet in backlog_inventory, f"backlog-inventory.md is missing {snippet!r}.", errors)

    for snippet in (
        "Implemented for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A",
        "`Wave-B` handled `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` after `C13` Wave A",
        "`Wave-C` handled `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` without reopening suite-wide vocabulary or the shared platform contract.",
    ):
        require(snippet in backlog_routing, f"backlog-routing.md is missing {snippet!r}.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Wave B surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
