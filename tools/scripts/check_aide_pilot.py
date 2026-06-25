"""Validate the report-only AIDE pilot control plane."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
PILOT_PATH = ROOT / ".aide" / "pilot.toml"
GITIGNORE_PATH = ROOT / ".gitignore"

REQUIRED_PATHS = [
    ROOT / ".aide" / "README.md",
    PILOT_PATH,
    ROOT / ".aide" / "work_units" / "truth-proof-baseline.toml",
    ROOT / ".aide" / "evidence_packets" / "README.md",
    ROOT / ".aide" / "guidance" / "codex.md",
]


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


def scan_runtime_dependency_references(errors: list[str]) -> None:
    for root_name in ("platform", "products"):
        root = ROOT / root_name
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix.lower() in {".png", ".jpg", ".jpeg", ".gif", ".ico", ".bmp"}:
                continue
            try:
                text = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            if ".aide" in text or "AIDE" in text:
                errors.append(f"Runtime/product tree must not reference AIDE during the pilot: {path.relative_to(ROOT)}")


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing AIDE pilot path: {path.relative_to(ROOT)}", errors)

    pilot = load_toml(PILOT_PATH)
    require(pilot.get("schema_version") == 1, ".aide/pilot.toml schema_version must be 1.", errors)
    require(pilot.get("status") == "report-only", ".aide/pilot.toml status must remain report-only.", errors)
    require(pilot.get("runtime_dependency_allowed") is False, "AIDE runtime dependency must be disabled.", errors)
    require(pilot.get("automatic_merging_allowed") is False, "AIDE automatic merging must be disabled.", errors)
    require(pilot.get("product_runtime_dependency_allowed") is False, "AIDE product runtime dependency must be disabled.", errors)

    for label, value in pilot.get("authority", {}).items():
        require_path(value, f"authority.{label}", errors)
    for label, value in pilot.get("validators", {}).items():
        require_path(value, f"validators.{label}", errors)
    for index, item in enumerate(pilot.get("work_units", [])):
        require_path(item.get("path"), f"work_units[{index}].path", errors)
    for index, item in enumerate(pilot.get("evidence_packets", [])):
        require_path(item.get("path"), f"evidence_packets[{index}].path", errors)

    gitignore = GITIGNORE_PATH.read_text(encoding="utf-8")
    require(".aide.local/" in gitignore, ".gitignore must ignore .aide.local/.", errors)

    scan_runtime_dependency_references(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE pilot checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
