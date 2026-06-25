"""Validate and summarize the canonical ScreenSave project-state authority."""

from __future__ import annotations

import argparse
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE_PATH = ROOT / "PROJECT_STATE.toml"
VERSION_PATH = ROOT / "VERSION.toml"
README_PATH = ROOT / "README.md"
VERSION_HEADER = ROOT / "platform" / "include" / "screensave" / "version.h"

REQUIRED_TOP_LEVEL = {
    "schema_version",
    "state_id",
    "as_of",
    "authority",
    "release",
    "development",
    "compatibility",
    "queues",
    "plasma",
    "build_profiles",
    "validators",
}

REQUIRED_QUEUE_STATUS = "historical-superseded"


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_path(value: object, label: str, errors: list[str]) -> None:
    if not isinstance(value, str) or not value:
        errors.append(f"{label} must be a non-empty path string.")
        return
    path = (ROOT / value).resolve()
    require(path.exists(), f"{label} does not exist: {value}", errors)


def validate_state(state: dict) -> list[str]:
    errors: list[str] = []

    missing = REQUIRED_TOP_LEVEL - set(state)
    for key in sorted(missing):
        errors.append(f"PROJECT_STATE.toml is missing top-level key {key!r}.")

    authority = state.get("authority", {})
    release = state.get("release", {})
    development = state.get("development", {})
    compatibility = state.get("compatibility", {})
    queues = state.get("queues", {})
    validators = state.get("validators", {})
    plasma = state.get("plasma", {})
    version = load_toml(VERSION_PATH)
    catalog = load_toml(ROOT / "catalog" / "products.toml")

    require(authority.get("current_truth") == "truth-proof-baseline", "authority.current_truth must be truth-proof-baseline.", errors)
    require(authority.get("public_release") == release.get("tag"), "authority.public_release must match release.tag.", errors)
    require(queues.get("status") == REQUIRED_QUEUE_STATUS, "queues.status must be historical-superseded.", errors)
    require(development.get("ship_posture") == "GO_WITH_CAVEATS", "development.ship_posture must preserve GO_WITH_CAVEATS.", errors)
    require(compatibility.get("policy") == "evidence-classed", "compatibility.policy must be evidence-classed.", errors)
    require(authority.get("version_manifest") == "VERSION.toml", "authority.version_manifest must point to VERSION.toml.", errors)

    for label, value in (
        ("authority.product_catalog", authority.get("product_catalog")),
        ("authority.artifact_profiles", authority.get("artifact_profiles")),
        ("authority.version_manifest", authority.get("version_manifest")),
        ("authority.aide_pilot", authority.get("aide_pilot")),
        ("release.artifact_manifest", release.get("artifact_manifest")),
        ("release.checksums", release.get("checksums")),
        ("release.notes", release.get("notes")),
        ("release.known_limits", release.get("known_limits")),
        ("development.current_product_truth", development.get("current_product_truth")),
        ("development.ship_posture_source", development.get("ship_posture_source")),
        ("development.proof_record", development.get("proof_record")),
        ("development.proof_captures", development.get("proof_captures")),
        ("compatibility.binary_audit", compatibility.get("binary_audit")),
        ("plasma.stable.source", plasma.get("stable", {}).get("source")),
        ("plasma.experimental.source", plasma.get("experimental", {}).get("source")),
    ):
        require_path(value, label, errors)

    wave_files = queues.get("wave_files")
    if not isinstance(wave_files, list) or not wave_files:
        errors.append("queues.wave_files must be a non-empty list.")
    else:
        for index, value in enumerate(wave_files):
            require_path(value, f"queues.wave_files[{index}]", errors)
            if isinstance(value, str):
                wave = load_toml(ROOT / value)
                require(wave.get("status") == REQUIRED_QUEUE_STATUS, f"{value} must have status {REQUIRED_QUEUE_STATUS!r}.", errors)

    for name, value in validators.items():
        require_path(value, f"validators.{name}", errors)

    validate_version_manifest(state, version, catalog, errors)

    readme = README_PATH.read_text(encoding="utf-8")
    for phrase in (
        state.get("state_id", ""),
        "Truth And Proof Baseline",
        "VERSION.toml",
        "GO WITH CAVEATS",
        "evidence-classed",
    ):
        require(isinstance(phrase, str) and phrase in readme, f"README.md must mention {phrase!r}.", errors)

    return errors


def validate_version_manifest(state: dict, version: dict, catalog: dict, errors: list[str]) -> None:
    release = state["release"]
    authority = state["authority"]
    compatibility = state["compatibility"]
    version_release = version.get("release", {})
    version_development = version.get("development", {})
    version_schemas = version.get("schemas", {})
    version_proof = version.get("proof", {})

    require(version.get("schema_version") == 1, "VERSION.toml schema_version must be 1.", errors)
    require(version_release.get("public_artifact") == release.get("tag"), "VERSION.toml release.public_artifact must match PROJECT_STATE release.tag.", errors)
    require(version_release.get("runtime_version") == "0.15.0", "VERSION.toml release.runtime_version must remain 0.15.0 for C16.", errors)
    require(version_release.get("status") == release.get("status"), "VERSION.toml release.status must match PROJECT_STATE release.status.", errors)
    require(version_development.get("current_state") == state.get("state_id"), "VERSION.toml development.current_state must match PROJECT_STATE state_id.", errors)
    require(version_development.get("development_head") == authority.get("development_head"), "VERSION.toml development.development_head must match PROJECT_STATE authority.", errors)
    require(version_schemas.get("product_catalog") == catalog.get("schema_version"), "VERSION.toml schemas.product_catalog must match catalog schema_version.", errors)
    require(version_proof.get("policy") == compatibility.get("policy"), "VERSION.toml proof.policy must match PROJECT_STATE compatibility.policy.", errors)
    require_path(version_proof.get("binary_audit_tool"), "VERSION.toml proof.binary_audit_tool", errors)
    require_path(version_proof.get("state_validator"), "VERSION.toml proof.state_validator", errors)

    header = VERSION_HEADER.read_text(encoding="utf-8")
    expected_runtime = version_release.get("runtime_version")
    expected_series = version_release.get("series")
    require(f'SCREENSAVE_VERSION_TEXT "{expected_runtime}"' in header, "version.h must match VERSION.toml release.runtime_version.", errors)
    require(f'SCREENSAVE_VERSION_SERIES "{expected_series}"' in header, "version.h must match VERSION.toml release.series.", errors)


def print_summary(state: dict) -> None:
    authority = state["authority"]
    release = state["release"]
    development = state["development"]
    compatibility = state["compatibility"]
    queues = state["queues"]
    plasma = state["plasma"]["stable"]

    print(f"State: {state['state_id']} ({state['as_of']})")
    print(f"Released public artifact: {release['tag']} [{release['status']}]")
    print(f"Development head: {authority['development_head']}")
    print(f"Active program: {authority['active_program']}")
    print(f"Release candidate: {authority['release_candidate']}")
    print(f"Queue authority: {queues['status']} ({queues['superseded_by']})")
    print(f"Compatibility policy: {compatibility['policy']} / default OS status: {compatibility['default_os_status']}")
    print(f"Version authority: {authority['version_manifest']}")
    print(
        "Plasma stable center: "
        f"{plasma['default_preset']} + {plasma['default_theme']}, "
        f"{plasma['minimum_kind']} -> {plasma['preferred_kind']}, {plasma['quality_class']}"
    )
    print(f"Plasma ship posture: {development['ship_posture']}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary", action="store_true", help="Print the current state summary after validation.")
    args = parser.parse_args()

    state = load_toml(STATE_PATH)
    errors = validate_state(state)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    if args.summary:
        print_summary(state)
    else:
        print("Project state checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
