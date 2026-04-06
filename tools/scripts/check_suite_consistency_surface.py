"""Validate the C12 suite-consistency surface and status docs."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

CANONICAL_SAVERS = (
    "nocturne",
    "ricochet",
    "deepfield",
    "plasma",
    "phosphor",
    "pipeworks",
    "lifeforms",
    "signals",
    "mechanize",
    "ecosystems",
    "stormglass",
    "transit",
    "observatory",
    "vector",
    "explorer",
    "city",
    "atlas",
    "gallery",
    "anthology",
)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def read_ini(path: pathlib.Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with path.open("r", encoding="utf-8") as handle:
        parser.read_file(handle)
    return parser


def main() -> int:
    errors: list[str] = []

    quality_bar_path = ROOT / "docs" / "roadmap" / "c12-suite-quality-bar.md"
    audit_path = ROOT / "validation" / "notes" / "c12-suite-consistency-audit.md"
    host_resource_path = ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc"
    host_diag_path = ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_diagnostics.c"
    suite_app_path = ROOT / "products" / "apps" / "suite" / "src" / "suite_app.c"
    suite_browser_path = ROOT / "products" / "apps" / "suite" / "src" / "suite_browser.c"
    suite_manifest_path = ROOT / "products" / "apps" / "suite" / "src" / "suite_manifest.c"
    benchlab_manifest_path = ROOT / "products" / "apps" / "benchlab" / "manifest.ini"
    benchlab_overlay_path = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_overlay.c"
    anthology_config_path = ROOT / "products" / "savers" / "anthology" / "src" / "anthology_config.rc"

    for path in (
        quality_bar_path,
        audit_path,
        host_resource_path,
        host_diag_path,
        suite_app_path,
        suite_browser_path,
        suite_manifest_path,
        benchlab_manifest_path,
        benchlab_overlay_path,
        anthology_config_path,
    ):
        require(path.exists(), f"Missing required C12 path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    quality_bar = read_text(quality_bar_path)
    audit = read_text(audit_path)
    host_resource = read_text(host_resource_path)
    host_diag = read_text(host_diag_path)
    suite_app = read_text(suite_app_path)
    suite_browser = read_text(suite_browser_path)
    suite_manifest = read_text(suite_manifest_path)
    benchlab_overlay = read_text(benchlab_overlay_path)
    anthology_config = read_text(anthology_config_path)
    benchlab_manifest = read_ini(benchlab_manifest_path)

    for snippet in (
        "# C12 Suite Quality Bar",
        "`General`",
        "`Appearance`",
        "`Behavior`",
        "`Randomization`",
        "`Advanced`",
        "`Renderer preference`",
        "`Session randomization`",
        "`Apply`",
        "`Revert Changes`",
        "`Defaults`",
        "`Selection path`",
        "`Fallback cause`",
        "`C13` Wave C and `C14` final rerelease hardening completed the continuation line without reopening the quality-bar vocabulary.",
        "Later post-release work should continue to preserve this quality bar.",
    ):
        require(snippet in quality_bar, f"c12-suite-quality-bar.md is missing {snippet!r}.", errors)

    require("# C12 Suite Consistency Audit" in audit, "C12 audit note is missing its title.", errors)
    require("- `C13` first saver-specific polish wave" in audit, "C12 audit note must name the next continuation step.", errors)

    require("Renderer preference" in host_resource, "Host resources must use the renderer preference label.", errors)
    require("Session randomization" in host_resource, "Host resources must use the session randomization label.", errors)

    for snippet in (
        "Detail level:",
        "Randomization mode:",
        "Preset key:",
        "Theme key:",
        "Renderer preference:",
        "Selection path:",
        "Fallback cause:",
        "Renderer status:",
    ):
        require(snippet in host_diag, f"scr_diagnostics.c is missing {snippet!r}.", errors)
        require(snippet in benchlab_overlay, f"benchlab_overlay.c is missing {snippet!r}.", errors)

    for snippet in (
        "Renderer preference",
        "Session randomization",
        "Apply",
        "Revert Changes",
        "Auto (best available)",
        "GDI floor",
        "OpenGL 1.1",
        "OpenGL 2.1",
    ):
        require(snippet in suite_app, f"suite_app.c is missing {snippet!r}.", errors)

    for snippet in (
        "Selected preset",
        "Selected theme",
        "Detail level",
        "Randomization mode",
        "Preview renderer preference",
        "Selection path",
        "Fallback cause",
        "Renderer status",
    ):
        require(snippet in suite_browser, f"suite_browser.c is missing {snippet!r}.", errors)

    for snippet in (
        "GDI floor",
        "OpenGL 1.1 optional",
        "OpenGL 2.1 optional",
        "Quiet and Foundation",
        "Framebuffer and Vector",
        "Grid and Simulation",
        "Systems and Ambient",
        "Places and Atmosphere",
    ):
        require(snippet in suite_manifest, f"suite_manifest.c is missing {snippet!r}.", errors)

    for snippet in (
        "Renderer policy",
        "Honor saver-local randomization",
    ):
        require(snippet in anthology_config, f"anthology_config.rc is missing {snippet!r}.", errors)

    available_savers = tuple(
        saver.strip()
        for saver in benchlab_manifest.get("identity", "available_savers", fallback="").split(",")
        if saver.strip()
    )
    require(
        available_savers == CANONICAL_SAVERS,
        "benchlab/manifest.ini must list the full canonical saver line plus anthology.",
        errors,
    )

    readme = read_text(ROOT / "README.md")
    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    changelog = read_text(ROOT / "CHANGELOG.md")

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
        "Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, the completed `C13` family-polish work, the completed `C14` release-hardening pass, and the completed `C15` release-doctrine and channel-split pass." in prompt_program,
        "prompt-program.md must record completed C13 family-polish work.",
        errors,
    )
    require(
        "`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, and `C15` release doctrine and channel split are complete. `C16` Core release refresh and baseline freeze is next." in prompt_program,
        "prompt-program.md must point to C15 and C16.",
        errors,
    )
    require(
        "- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C14`, and `C15` are complete." in prompt_program,
        "prompt-program.md must record the completed C00-C12, C14, and C15 continuation line.",
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
        "13. `C12` completed the cross-cutting polish and quality-bar pass before the first `C13` saver-specific polish wave begins." in post_s15,
        "post-s15-plan.md must record C12 as complete.",
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
    require("## C12 - 2026-03-30" in changelog, "CHANGELOG.md must record C12.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Suite consistency surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
