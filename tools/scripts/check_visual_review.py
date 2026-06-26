"""Validate Plasma v2 Visual Review v1 scaffolding."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "visual_review_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-visual-review.md"
PROTOCOL = ROOT / "validation" / "captures" / "plasma-v2" / "review" / "review-protocol.json"
CONTACT_MANIFEST = ROOT / "validation" / "captures" / "plasma-v2" / "review" / "contact-sheet-manifest.json"
DECISION_TEMPLATE = ROOT / "validation" / "captures" / "plasma-v2" / "review" / "reviewer-decision.template.toml"

REQUIRED_DIMENSIONS = {
    "composition",
    "motion_quality",
    "palette_material_quality",
    "luminance_safety",
    "dark_room_comfort",
    "control_influence",
    "distinctness",
    "artifacting",
    "degradation_honesty",
    "preview_fullscreen_suitability",
}
DECISION_CLASSES = {"accepted-for-preview", "request-changes", "rejected", "defer-to-labs"}
NON_DECISIONS = {
    "stable_release",
    "compatibility_certification",
    "core_promotion",
    "all_platform_support",
    "public_sdk_stability",
    "automatic_product_promotion",
}
BOUNDARY_TEXT = "does not decide stable release"


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_path(path: pathlib.Path, label: str, errors: list[str]) -> None:
    require(path.exists(), f"Missing {label}: {path.relative_to(ROOT)}", errors)


def main() -> int:
    errors: list[str] = []

    for path, label in (
        (CONTRACT, "visual review contract"),
        (PLASMA_DOC, "Plasma visual review doc"),
        (PROTOCOL, "review protocol"),
        (CONTACT_MANIFEST, "contact sheet manifest"),
        (DECISION_TEMPLATE, "reviewer decision template"),
    ):
        require_path(path, label, errors)

    if not errors:
        contract_text = CONTRACT.read_text(encoding="utf-8")
        for needle in (
            "Status: active preview-review contract.",
            "accepted-for-preview",
            "stable release",
            "compatibility certification",
            "development-plane evidence is not ScreenSave product truth",
        ):
            require(needle in contract_text, f"contracts/visual_review_v1.md is missing {needle!r}", errors)

        plasma_doc = PLASMA_DOC.read_text(encoding="utf-8")
        require("stable or released" in plasma_doc, "Plasma review doc must preserve non-release boundary.", errors)
        require("compatibility certification" in plasma_doc, "Plasma review doc must name certification as a non-decision.", errors)
        require("AIDE" not in plasma_doc, "Product visual review doc must not introduce AIDE references.", errors)

        protocol = load_json(PROTOCOL)
        require(protocol.get("schema_version") == "screensave.visual_review.v1", "review protocol schema_version mismatch.", errors)
        require(set(protocol.get("dimensions", [])) == REQUIRED_DIMENSIONS, "review protocol dimensions must match Visual Review v1.", errors)
        require(set(protocol.get("decision_classes", [])) == DECISION_CLASSES, "review protocol decision classes mismatch.", errors)
        require(set(protocol.get("non_decisions", [])) == NON_DECISIONS, "review protocol non-decisions mismatch.", errors)
        require(BOUNDARY_TEXT in protocol.get("claim_boundary", ""), "review protocol must preserve release boundary.", errors)

        manifest = load_json(CONTACT_MANIFEST)
        require(manifest.get("schema_version") == "screensave.visual_review_contact_sheet_manifest.v1", "contact manifest schema_version mismatch.", errors)
        for ref in manifest.get("proof_refs", []):
            require((ROOT / ref).exists(), f"contact manifest proof ref does not exist: {ref}", errors)
        for sheet in manifest.get("contact_sheets", []):
            require((ROOT / sheet.get("path", "")).exists(), f"contact sheet ref does not exist: {sheet.get('path')}", errors)

        template = load_toml(DECISION_TEMPLATE)
        require(template.get("schema_version") == 1, "reviewer decision template schema_version must be 1.", errors)
        require(template.get("template") is True, "reviewer decision template must mark template=true.", errors)
        require(set(template.get("dimensions", {}).keys()) == REQUIRED_DIMENSIONS, "reviewer template dimensions mismatch.", errors)
        require(set(template.get("explicit_non_decisions", [])) >= {"stable_release", "compatibility_certification"}, "reviewer template must list release and certification as non-decisions.", errors)
        require("Preview review only" in template.get("claim_boundary", ""), "reviewer template must preserve preview-only claim boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Visual review checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
