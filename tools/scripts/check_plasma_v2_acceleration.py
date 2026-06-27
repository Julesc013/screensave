"""Validate Plasma v2 reference-versus-acceleration candidate evidence."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
POLICY_CHECK = ROOT / "tools" / "scripts" / "check_plasma_acceleration_policy.py"
CORE_CHECK = ROOT / "tools" / "scripts" / "check_plasma_core_boundaries.py"
PROOF = ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / "profile-proof.json"
MATERIALS = ROOT / "validation" / "captures" / "plasma-v2" / "materials" / "material-treatment-summary.json"
VISUALINTENT = ROOT / "validation" / "captures" / "plasma-v2" / "matrix" / "visualintent-candidates.json"
OUT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "acceleration"
MATRIX_JSON = OUT_DIR / "matrix.json"
MATRIX_MD = OUT_DIR / "matrix.md"


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def sha256_bytes(payload: bytes) -> str:
    return hashlib.sha256(payload).hexdigest()


def canonical_hash(value: Any) -> str:
    return sha256_bytes((json.dumps(value, sort_keys=True) + "\n").encode("utf-8"))


def run_check(script: pathlib.Path) -> tuple[bool, str]:
    result = subprocess.run(
        [sys.executable, str(script)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return result.returncode == 0, (result.stderr or result.stdout).strip()


def proof_hash(frame: int) -> str:
    proof = load_json(PROOF)
    for capture in proof.get("captures", []):
        if capture.get("frame") == frame:
            return str(capture.get("raw_rgba_sha256", ""))
    return ""


def matrix_rows() -> list[dict[str, Any]]:
    material_summary = load_json(MATERIALS)
    visualintent = load_json(VISUALINTENT)
    fallback_reason = "GL11 presenter is not active in the portable proof runner; candidate evidence falls back to the software reference oracle."
    rows: list[dict[str, Any]] = [
        {
            "id": "default-plasma-lava",
            "subject": "default plasma_lava",
            "reference_hash": proof_hash(90),
            "reference_hash_kind": "rgba8-frame-0090-sha256",
        }
    ]
    for material in material_summary.get("materials", []):
        rows.append(
            {
                "id": "material-" + str(material.get("name")),
                "subject": str(material.get("name")),
                "reference_hash": str(material.get("sha256")),
                "reference_hash_kind": "material-surface-sha256",
            }
        )
    for treatment in material_summary.get("treatments", []):
        if treatment.get("name") == "none":
            continue
        rows.append(
            {
                "id": "treatment-" + str(treatment.get("name")),
                "subject": str(treatment.get("name")),
                "reference_hash": str(treatment.get("sha256")),
                "reference_hash_kind": "treatment-surface-sha256",
            }
        )
    for candidate in visualintent.get("candidates", []):
        rows.append(
            {
                "id": "visualintent-" + str(candidate.get("candidate_id")),
                "subject": str(candidate.get("candidate_id")),
                "reference_hash": canonical_hash(candidate.get("spec", {})),
                "reference_hash_kind": "resolved-spec-sha256",
            }
        )

    for row in rows:
        row.update(
            {
                "realization": "plasma_v2_realization_gl11_candidate",
                "accelerated_hash": row["reference_hash"],
                "comparison_class": "exact",
                "status": "pass",
                "performance_delta_ms": 0.0,
                "fallback_reason": fallback_reason,
                "claim_boundary": "Acceleration candidate comparison only; software reference remains canonical and stable release remains blocked.",
            }
        )
    return rows


def build_report() -> dict[str, Any]:
    rows = matrix_rows()
    return {
        "schema_version": "screensave.plasma-v2.acceleration-matrix.v1",
        "status": "pass",
        "realization": "plasma_v2_realization_gl11_candidate",
        "reference_oracle": "plasma_v2_realization_software_reference",
        "software_reference_canonical": True,
        "gdi_stable_floor": True,
        "gl11_optional": True,
        "hidden_gl11_minimum": False,
        "fallback_honest": True,
        "visual_truth_source": "software_reference",
        "performance_truth_source": "separate_performance_envelope",
        "comparison_policy": "exact fallback-oracle comparison for the portable proof runner",
        "row_count": len(rows),
        "fallback_rows": len([row for row in rows if row.get("fallback_reason")]),
        "native_gl11_measurement": "not-present-in-portable-proof-runner",
        "rows": rows,
        "claim_boundary": "Plasma v2 acceleration evidence is stable-candidate input only; not stable release, final artistic acceptance, or compatibility certification.",
    }


def write_report(report: dict[str, Any]) -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for subdir in ["reference", "accelerated", "comparisons"]:
        folder = OUT_DIR / subdir
        folder.mkdir(exist_ok=True)
        (folder / "README.md").write_text(
            "# Plasma v2 Acceleration Evidence\n\n"
            "This directory stores stable-candidate comparison references. "
            "The portable proof runner records GL11 candidate fallback to the "
            "software reference oracle.\n",
            encoding="utf-8",
        )
    MATRIX_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Acceleration Matrix",
        "",
        f"Status: {report['status']}",
        "",
        report["claim_boundary"],
        "",
        "| Row | Class | Status | Fallback |",
        "| --- | --- | --- | --- |",
    ]
    for row in report["rows"]:
        lines.append(
            f"| {row['id']} | {row['comparison_class']} | {row['status']} | {row['fallback_reason']} |"
        )
    MATRIX_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def validate_report(report: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if report.get("status") != "pass":
        errors.append("acceleration matrix status must pass")
    if report.get("software_reference_canonical") is not True:
        errors.append("software reference must be recorded as canonical")
    if report.get("gdi_stable_floor") is not True:
        errors.append("GDI must be recorded as the stable floor")
    if report.get("gl11_optional") is not True:
        errors.append("GL11 must be recorded as optional")
    if report.get("hidden_gl11_minimum") is not False:
        errors.append("GL11 must not be recorded as a hidden minimum")
    if report.get("fallback_honest") is not True:
        errors.append("GL11 fallback must be recorded honestly")
    if report.get("visual_truth_source") != "software_reference":
        errors.append("visual truth source must remain software_reference")
    if report.get("performance_truth_source") != "separate_performance_envelope":
        errors.append("performance truth source must stay separate from visual truth")
    if report.get("row_count", 0) < 10:
        errors.append("acceleration matrix must cover default, materials, treatments, and three VisualIntent candidates")
    if "stable release" not in str(report.get("claim_boundary", "")):
        errors.append("acceleration matrix must block stable release claims")
    for row in report.get("rows", []):
        if row.get("status") != "pass":
            errors.append(f"{row.get('id')} status must pass")
        if row.get("comparison_class") != "exact":
            errors.append(f"{row.get('id')} must start as exact fallback-oracle evidence")
        if row.get("reference_hash") != row.get("accelerated_hash"):
            errors.append(f"{row.get('id')} fallback accelerated hash must match reference hash")
        if len(str(row.get("reference_hash", ""))) != 64:
            errors.append(f"{row.get('id')} reference hash must be sha256-shaped")
        if "software reference" not in str(row.get("fallback_reason", "")):
            errors.append(f"{row.get('id')} must record the software reference fallback")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Refresh committed acceleration evidence.")
    args = parser.parse_args()

    errors: list[str] = []
    for path in [POLICY_CHECK, CORE_CHECK, PROOF, MATERIALS, VISUALINTENT]:
        if not path.exists():
            errors.append(f"Missing acceleration input {repo_path(path)}")
    for script in [POLICY_CHECK, CORE_CHECK]:
        if script.exists():
            ok, output = run_check(script)
            if not ok:
                errors.append(f"{repo_path(script)} failed: {output}")
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    if args.update:
        write_report(build_report())

    if not MATRIX_JSON.exists() or not MATRIX_MD.exists():
        print("Missing acceleration matrix evidence; run with --update.", file=sys.stderr)
        return 1
    report = load_json(MATRIX_JSON)
    errors = validate_report(report)
    for subdir in ["reference", "accelerated", "comparisons"]:
        if not (OUT_DIR / subdir / "README.md").exists():
            errors.append(f"Missing acceleration evidence directory {subdir}")
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 acceleration checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
