"""Validate the Plasma v2 reference preview gate."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
PROOF_PROFILES = ROOT / "catalog" / "proof_profiles.toml"
GATE_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "preview-gate"
GATE_JSON = GATE_DIR / "gate-report.json"
GATE_MD = GATE_DIR / "gate-report.md"


REQUIRED_PATHS = {
    "plasma_spec_contract": ROOT / "contracts" / "plasma_spec_v2.md",
    "pack_contract": ROOT / "contracts" / "pack_v1.md",
    "migration_header": ROOT / "products" / "savers" / "plasma" / "src" / "plasma_migration_v2.h",
    "migration_source": ROOT / "products" / "savers" / "plasma" / "src" / "plasma_migration_v2.c",
    "core_header": ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_core.h",
    "core_source": ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_core.c",
    "pack_example": ROOT / "products" / "savers" / "plasma" / "content" / "v2" / "examples" / "plasma_lava_v2.toml",
    "profile_proof": ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / "profile-proof.json",
    "proof_bundle": ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / "proof-bundle-v1.json",
    "material_summary": ROOT / "validation" / "captures" / "plasma-v2" / "materials" / "material-treatment-summary.json",
    "aide_packet": ROOT / ".aide" / "evidence_packets" / "screensave-plasma-v2-reference-preview.toml",
}

REQUIRED_CAPTURE_FRAMES = [0, 8, 32, 90]


def repo_path(path: pathlib.Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path)


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_check(args: list[str], timeout_seconds: int = 240) -> dict[str, Any]:
    result = subprocess.run(
        [sys.executable, *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout_seconds,
        check=False,
    )
    return {
        "command": " ".join(args),
        "status": "pass" if result.returncode == 0 else "fail",
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
    }


def add_result(results: list[dict[str, Any]], result_id: str, status: bool, summary: str, **payload: Any) -> None:
    results.append(
        {
            "id": result_id,
            "status": "pass" if status else "fail",
            "summary": summary,
            "payload": payload,
        }
    )


def proof_profile() -> dict[str, Any]:
    data = load_toml(PROOF_PROFILES)
    for item in data.get("proof_profiles", []):
        if item.get("key") == "plasma.v2.reference.preview":
            return item
    return {}


def blocked_claim_scan() -> dict[str, Any]:
    paths = [
        ROOT / "PROJECT_STATE.toml",
        ROOT / "README.md",
        ROOT / "VERSION.toml",
        ROOT / "docs" / "roadmap" / "plasma-v2-reference-slice.md",
        ROOT / ".aide" / "evidence_packets" / "screensave-plasma-v2-reference-preview.toml",
    ]
    forbidden = [
        "stable = true",
        "artistic_acceptance = \"accepted\"",
        "release_promotion = \"promoted\"",
        "compatibility_certification = \"certified\"",
        "compatibility certified",
    ]
    hits: list[str] = []
    for path in paths:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="ignore").lower()
        for token in forbidden:
            if token in text:
                hits.append(f"{repo_path(path)} contains {token}")
    return {"status": "pass" if not hits else "fail", "hits": hits}


def build_report() -> dict[str, Any]:
    results: list[dict[str, Any]] = []
    state = load_toml(STATE)
    portable_v2 = state.get("portable_v2", {})
    development = state.get("development", {})
    plasma_v2 = state.get("plasma_v2", {})

    add_result(
        results,
        "portable-v2-accepted",
        portable_v2.get("status") == "accepted",
        "Portable v2 must be accepted before Plasma v2 preview.",
        observed_status=portable_v2.get("status"),
    )
    add_result(
        results,
        "active-program",
        development.get("active_program") == "plasma-v2-reference-slice",
        "Active program must be plasma-v2-reference-slice.",
        active_program=development.get("active_program"),
    )
    if plasma_v2:
        add_result(
            results,
            "plasma-v2-state-boundary",
            plasma_v2.get("stable") is False
            and plasma_v2.get("release_promotion") == "blocked"
            and plasma_v2.get("artistic_acceptance") != "accepted",
            "Plasma v2 state, when present, must remain preview-only.",
            plasma_v2=plasma_v2,
        )
    else:
        add_result(
            results,
            "plasma-v2-state-boundary",
            True,
            "Plasma v2 state is not yet recorded; preview gate remains pre-state.",
        )

    for path_id, path in REQUIRED_PATHS.items():
        add_result(results, f"path-{path_id}", path.exists(), f"Required path exists: {repo_path(path)}")

    profile = proof_profile()
    add_result(
        results,
        "proof-profile-implemented",
        profile.get("status") == "implemented" and profile.get("product") == "plasma",
        "Plasma preview proof profile must be implemented.",
        profile=profile,
    )
    if (ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview").exists():
        missing = []
        for frame in REQUIRED_CAPTURE_FRAMES:
            for suffix in ["rgba", "ppm"]:
                path = ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / f"frame-{frame:04d}.{suffix}"
                if not path.exists():
                    missing.append(repo_path(path))
        add_result(results, "reference-captures", not missing, "Reference preview captures must exist.", missing=missing)

    if REQUIRED_PATHS["profile_proof"].exists():
        proof = load_json(REQUIRED_PATHS["profile_proof"])
        add_result(
            results,
            "profile-proof-pass",
            proof.get("status") == "pass" and proof.get("comparison_status") == "pass",
            "Plasma profile proof must pass exact comparison.",
            observed_status=proof.get("status"),
            comparison_status=proof.get("comparison_status"),
        )
    if REQUIRED_PATHS["proof_bundle"].exists():
        bundle = load_json(REQUIRED_PATHS["proof_bundle"])
        axes = bundle.get("result_axes", {})
        add_result(
            results,
            "proof-bundle-boundaries",
            axes.get("capture", {}).get("status") == "pass"
            and axes.get("artistic_review", {}).get("status") == "blocked"
            and axes.get("release_promotion", {}).get("status") == "blocked",
            "Proof Bundle v1 must pass capture while blocking artistic/release axes.",
        )

    commands = [
        ("plasma-core-boundary", ["tools/scripts/check_plasma_core_boundaries.py"], 240),
        ("packc", ["tools/scripts/check_packc.py"], 120),
        ("plasma-spec", ["tools/scripts/check_plasma_spec_v2.py"], 120),
        ("plasma-materials", ["tools/scripts/check_plasma_v2_materials.py"], 240),
        ("workbench", ["tools/scripts/check_workbench_shell.py"], 300),
        ("aide-evidence-bridge", ["tools/scripts/check_aide_evidence_bridge.py"], 300),
        ("aide-evidence-index", ["tools/scripts/check_aide_evidence_index.py"], 120),
    ]
    for result_id, command, timeout_seconds in commands:
        run = run_check(command, timeout_seconds=timeout_seconds)
        add_result(results, f"validator-{result_id}", run.get("status") == "pass", "Required validator must pass.", run=run)

    scan = blocked_claim_scan()
    add_result(results, "blocked-claim-scan", scan.get("status") == "pass", "No stable/artistic/release/compatibility overclaim may exist.", hits=scan.get("hits", []))

    passed = all(result.get("status") == "pass" for result in results)
    return {
        "schema": "screensave.plasma-v2.preview-gate.v0",
        "status": "pass" if passed else "fail",
        "profile": "plasma.v2.reference.preview",
        "claim_boundary": "Plasma v2 reference preview candidate only; not stable release promotion, artistic acceptance, or compatibility certification.",
        "results": results,
    }


def write_reports(report: dict[str, Any]) -> None:
    GATE_DIR.mkdir(parents=True, exist_ok=True)
    GATE_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Preview Gate",
        "",
        f"Status: {report.get('status')}",
        "",
        report.get("claim_boundary", ""),
        "",
        "| Check | Status | Summary |",
        "| --- | --- | --- |",
    ]
    for result in report.get("results", []):
        lines.append(f"| {result.get('id')} | {result.get('status')} | {result.get('summary')} |")
    GATE_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    report = build_report()
    write_reports(report)
    if report.get("status") != "pass":
        for result in report.get("results", []):
            if result.get("status") != "pass":
                print(f"{result.get('id')}: {result.get('summary')}", file=sys.stderr)
        return 1
    print(f"Plasma v2 preview gate passed: {repo_path(GATE_JSON)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
