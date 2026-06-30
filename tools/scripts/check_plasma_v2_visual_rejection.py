"""Validate the Plasma v2 visual-rejection publication hold."""

from __future__ import annotations

import hashlib
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
VERSION = ROOT / "VERSION.toml"
VERDICT = ROOT / "validation" / "captures" / "plasma-v2" / "visual-rejection" / "verdict.toml"
ROADMAP = ROOT / "docs" / "roadmap" / "plasma-v2-publication-hold.md"
V3_RESET = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v3-visual-core-reset.md"
V3_PROOF = ROOT / "validation" / "captures" / "plasma-v3" / "visual-spike" / "proof-summary.json"

EXPECTED_REASONS = {
    "low-resolution",
    "blocky",
    "stuttery",
    "not visually acceptable",
}


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_path(path: pathlib.Path, label: str, errors: list[str]) -> None:
    require(path.exists(), f"{label} missing: {repo_path(path)}", errors)


def validate_state_block(label: str, block: dict[str, Any], errors: list[str]) -> None:
    require(block.get("status") == "publication-hold", f"{label}.status must be publication-hold.", errors)
    require(block.get("stable") is False, f"{label}.stable must be false.", errors)
    require(block.get("release_candidate") == "plasma-v2-rc1", f"{label}.release_candidate must be plasma-v2-rc1.", errors)
    require(
        block.get("stable_promotion_historical") == "accepted",
        f"{label}.stable_promotion_historical must preserve the historical accepted promotion.",
        errors,
    )
    require(
        block.get("current_product_verdict") == "visual-rejected",
        f"{label}.current_product_verdict must be visual-rejected.",
        errors,
    )
    require(
        block.get("release_promotion") == "withdrawn-for-visual-quality",
        f"{label}.release_promotion must be withdrawn-for-visual-quality.",
        errors,
    )
    require(block.get("publication_prep") == "superseded", f"{label}.publication_prep must be superseded.", errors)
    require(block.get("publication") == "not-published", f"{label}.publication must be not-published.", errors)
    require(
        block.get("opened_next") == "plasma-v3-visual-core-spike",
        f"{label}.opened_next must be plasma-v3-visual-core-spike.",
        errors,
    )
    require(
        block.get("visual_blocker") == "current artifact fails real-display visual acceptance",
        f"{label}.visual_blocker must record the real-display acceptance blocker.",
        errors,
    )


def validate_verdict(verdict: dict[str, Any], errors: list[str]) -> None:
    require(verdict.get("schema") == "screensave.plasma-v2.visual-rejection.verdict.v1", "verdict schema mismatch.", errors)
    require(verdict.get("candidate_id") == "plasma-v2-rc1", "verdict candidate_id must be plasma-v2-rc1.", errors)
    require(verdict.get("profile") == "plasma.v2.reference.preview", "verdict profile must be plasma.v2.reference.preview.", errors)
    artifact = verdict.get("artifact", {})
    artifact_path = ROOT / str(artifact.get("path", ""))
    require_path(artifact_path, "verdict artifact", errors)
    if artifact_path.exists():
        actual = sha256_file(artifact_path)
        require(
            actual == str(artifact.get("sha256", "")).lower(),
            f"verdict artifact sha256 mismatch for {repo_path(artifact_path)}.",
            errors,
        )

    visual = verdict.get("visual_verdict", {})
    require(visual.get("decision") == "reject-publication", "visual_verdict.decision must reject publication.", errors)
    require(
        visual.get("current_product_verdict") == "visual-rejected",
        "visual_verdict.current_product_verdict must be visual-rejected.",
        errors,
    )
    require(visual.get("real_display_required") is True, "visual_verdict.real_display_required must be true.", errors)
    require(visual.get("real_display_verdict") == "rejected", "visual_verdict.real_display_verdict must be rejected.", errors)
    require(visual.get("publication") == "not-published", "visual_verdict.publication must be not-published.", errors)
    require(
        visual.get("opened_next") == "plasma-v3-visual-core-spike",
        "visual_verdict.opened_next must be plasma-v3-visual-core-spike.",
        errors,
    )
    require(
        EXPECTED_REASONS <= set(visual.get("reasons", [])),
        "visual_verdict.reasons must include low-resolution, blocky, stuttery, and not visually acceptable.",
        errors,
    )

    transition = verdict.get("state_transition", {})
    require(transition.get("to_status") == "publication-hold", "state_transition.to_status must be publication-hold.", errors)
    require(transition.get("stable") is False, "state_transition.stable must be false.", errors)
    require(
        transition.get("release_promotion") == "withdrawn-for-visual-quality",
        "state_transition.release_promotion must be withdrawn-for-visual-quality.",
        errors,
    )
    require(transition.get("preserve_screen_save") is True, "state_transition.preserve_screen_save must be true.", errors)
    require(
        transition.get("restart_plasma_visual_core") is True,
        "state_transition.restart_plasma_visual_core must be true.",
        errors,
    )

    boundaries = verdict.get("boundaries", {})
    for key in (
        "publication_ready_is_not_publication",
    ):
        require(boundaries.get(key) is True, f"boundaries.{key} must be true.", errors)
    for key in (
        "screen_save_restart",
        "compatibility_broadening",
        "public_upload",
        "github_release",
        "manager_install_apply",
        "workbench_runtime_authority",
    ):
        require(boundaries.get(key) is False, f"boundaries.{key} must be false.", errors)


def validate_docs(errors: list[str]) -> None:
    require_path(ROADMAP, "publication hold roadmap", errors)
    require_path(V3_RESET, "Plasma V3 reset doc", errors)
    require_path(V3_PROOF, "Plasma V3 proof summary", errors)
    if ROADMAP.exists():
        text = ROADMAP.read_text(encoding="utf-8")
        for phrase in (
            "Do not publish Plasma v2.",
            "Preserve ScreenSave.",
            "Restart Plasma's visual core.",
            "status = \"publication-hold\"",
            "current_product_verdict = \"visual-rejected\"",
            "plasma-v3-visual-core-spike",
            "Green package, checksum, provenance, or publication-prep evidence is not enough",
        ):
            require(phrase in text, f"roadmap must mention {phrase!r}.", errors)


def main() -> int:
    errors: list[str] = []
    require_path(VERDICT, "visual rejection verdict", errors)
    require_path(STATE, "PROJECT_STATE.toml", errors)
    require_path(VERSION, "VERSION.toml", errors)

    if VERDICT.exists():
        validate_verdict(load_toml(VERDICT), errors)
    if STATE.exists():
        state = load_toml(STATE)
        authority = state.get("authority", {})
        development = state.get("development", {})
        plasma = state.get("plasma_v2", {})
        require(
            authority.get("active_program") == "plasma-v3-visual-core-spike",
            "authority.active_program must be plasma-v3-visual-core-spike.",
            errors,
        )
        require(
            development.get("active_program") == "plasma-v3-visual-core-spike",
            "development.active_program must be plasma-v3-visual-core-spike.",
            errors,
        )
        require(
            development.get("current_product_truth") == "docs/roadmap/plasma-v2-publication-hold.md",
            "development.current_product_truth must point to the publication hold roadmap.",
            errors,
        )
        validate_state_block("PROJECT_STATE.toml plasma_v2", plasma, errors)
        for key, relative in (
            ("visual_rejection_verdict", "validation/captures/plasma-v2/visual-rejection/verdict.toml"),
            ("publication_hold", "docs/roadmap/plasma-v2-publication-hold.md"),
            ("plasma_v3_visual_core_reset", "products/savers/plasma/docs/plasma-v3-visual-core-reset.md"),
            ("plasma_v3_visual_spike_proof", "validation/captures/plasma-v3/visual-spike/proof-summary.json"),
        ):
            require(plasma.get(key) == relative, f"PROJECT_STATE.toml plasma_v2.{key} must point to {relative}.", errors)
    if VERSION.exists():
        version = load_toml(VERSION)
        require(
            version.get("development", {}).get("active_milestone") == "plasma-v3-visual-core-spike",
            "VERSION.toml development.active_milestone must be plasma-v3-visual-core-spike.",
            errors,
        )
        validate_state_block("VERSION.toml plasma_v2", version.get("plasma_v2", {}), errors)

    validate_docs(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 visual-rejection hold checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
