"""Validate the C11 backlog and routing surface."""

from __future__ import annotations

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

NON_SAVER_PRODUCTS = ("suite", "benchlab", "sdk")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    routing_path = ROOT / "docs" / "roadmap" / "backlog-routing.md"
    inventory_path = ROOT / "docs" / "roadmap" / "backlog-inventory.md"
    threads_path = ROOT / "docs" / "roadmap" / "refinement-threads.md"
    baseline_path = ROOT / "validation" / "notes" / "c11-backlog-routing-baseline.md"

    for path in (routing_path, inventory_path, threads_path, baseline_path):
        require(path.exists(), f"Missing required C11 path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    routing = read_text(routing_path)
    inventory = read_text(inventory_path)
    threads = read_text(threads_path)
    baseline = read_text(baseline_path)
    readme = read_text(ROOT / "README.md")
    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    changelog = read_text(ROOT / "CHANGELOG.md")

    for snippet in (
        "Saver-local polish",
        "Shared-platform/runtime",
        "Renderer-tier work",
        "Suite/meta layer",
        "Release/distribution",
        "Docs/SDK/tooling",
        "Validation/hardening",
        "Deferred/parked ideas",
        "`priority`",
        "`effort`",
        "`risk`",
        "`depends_on`",
        "`target_bucket`",
        "`owner_area`",
        "`C12-X1`",
        "`Wave-A`",
        "`Wave-B`",
        "`Wave-C`",
    ):
        require(snippet in routing, f"backlog-routing.md is missing {snippet!r}.", errors)

    for saver in CANONICAL_SAVERS:
        require(f"`{saver}`" in inventory, f"backlog-inventory.md must cover saver {saver}.", errors)

    for product in NON_SAVER_PRODUCTS:
        require(f"`{product}`" in inventory, f"backlog-inventory.md must cover non-saver product {product}.", errors)

    for snippet in (
        "Master / Orchestration Thread",
        "Shared-Platform Thread",
        "Renderer Thread",
        "Release / Distribution Thread",
        "Suite / Meta Coordination Thread",
        "Per-Saver Refinement Threads",
        "Decision Return Path",
    ):
        require(snippet in threads, f"refinement-threads.md is missing {snippet!r}.", errors)

    require("# C11 Backlog Routing Baseline" in baseline, "C11 baseline note must use the correct title.", errors)
    require("- `C12` cross-cutting polish pass" in baseline, "C11 baseline note must name the next continuation step.", errors)

    require(
        "Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`." in readme,
        "README.md must record C16 as complete.",
        errors,
    )
    require(
        "Post-`C16` work now proceeds as a short `SS` release-closure series first:" in readme,
        "README.md must record the post-C16 handoff.",
        errors,
    )
    require(
        "Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, the completed `C13` family-polish work, the completed `C14` release-hardening pass, the completed `C15` release-doctrine and channel-split pass, and the completed `C16` Core baseline freeze." in prompt_program,
        "prompt-program.md must record C16 as complete.",
        errors,
    )
    require(
        "`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program,
        "prompt-program.md must record the completed C16 freeze.",
        errors,
    )
    require(
        "- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C14`, `C15`, and `C16` are complete." in prompt_program,
        "prompt-program.md must record the completed C00-C12, C14-C16 continuation line.",
        errors,
    )
    require(
        "`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16` are complete." in series_map,
        "series-map.md must record C16 as complete.",
        errors,
    )
    require(
        "`SS` runs first as the short release-closure bridge." in series_map,
        "series-map.md must point to the post-C16 handoff.",
        errors,
    )
    require(
        "12. `C11` completed structured backlog ingestion and routing before `C12` cross-cutting polish begins." in post_s15,
        "post-s15-plan.md must record C11 as complete.",
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
        "This file remains as historical record for the completed bridge; active post-`C16` planning now runs `SS` first and `PL` second from the frozen Core baseline." in post_s15,
        "post-s15-plan.md must point to the post-C16 handoff.",
        errors,
    )
    require("## C11 - 2026-03-30" in changelog, "CHANGELOG.md must record C11.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Backlog surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

