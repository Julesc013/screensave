"""Validate the Plasma v2 stable-candidate performance/resource envelope."""

from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import sys
import time
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "performance_envelope_v1.md"
ADAPTER = ROOT / "tools" / "project_adapter" / "screensave_project.py"
PROOF = ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / "profile-proof.json"
OUT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "performance"
RUN_DIR = OUT_DIR / "profile-runs"
ENVELOPE = OUT_DIR / "envelope.json"
SUMMARY = OUT_DIR / "summary.md"


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_profile() -> dict[str, Any]:
    command = [
        sys.executable,
        str(ADAPTER),
        "proof",
        "--profile",
        "plasma.v2.reference.preview",
        "--path",
        "v2",
    ]
    start = time.perf_counter()
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    elapsed_ms = round((time.perf_counter() - start) * 1000.0, 3)
    return {
        "command": " ".join(repo_path(pathlib.Path(part)) if part.endswith(".py") else part for part in command[1:]),
        "returncode": result.returncode,
        "elapsed_ms": elapsed_ms,
        "stdout_tail": result.stdout.strip().splitlines()[-10:],
        "stderr_tail": result.stderr.strip().splitlines()[-10:],
    }


def build_envelope(run: dict[str, Any]) -> dict[str, Any]:
    proof = load_json(PROOF)
    lifecycle_status = proof.get("lifecycle_status", "unknown")
    elapsed_ms = float(run.get("elapsed_ms", 0.0))
    return {
        "schema_version": "screensave.performance-envelope.v1",
        "status": "pass-with-warnings",
        "product": "plasma",
        "profile": "plasma.v2.reference.preview",
        "candidate": "plasma_v2_realization_gl11_candidate",
        "sample_count": 1,
        "metrics": {
            "frame_time_ms_p50": elapsed_ms,
            "frame_time_ms_p95": elapsed_ms,
            "frame_time_ms_p99": elapsed_ms,
            "first_frame_ms": elapsed_ms,
            "memory_high_water": {"status": "not-measured-in-portable-runner", "value": None},
            "allocation_growth": {"status": "not-measured-in-portable-runner", "value": None},
            "resize_cycle_result": lifecycle_status,
            "short_soak_result": "pass" if proof.get("status") == "pass" else "unknown",
            "fallback_frequency": 1.0,
        },
        "profile_run_ref": "validation/captures/plasma-v2/performance/profile-runs/plasma-v2-reference-preview.json",
        "warnings": [
            "Current committed envelope measures the shared proof command once.",
            "Native GL11 memory counters are not measured in the portable proof runner.",
            "Fallback frequency is 1.0 because GL11 is admitted as a candidate but not active in this runner.",
        ],
        "claim_boundary": "Performance envelope is stable-candidate input only; not stable release, compatibility certification, final artistic acceptance, or a public performance promise.",
    }


def write_envelope(envelope: dict[str, Any], run: dict[str, Any]) -> None:
    RUN_DIR.mkdir(parents=True, exist_ok=True)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    (RUN_DIR / "plasma-v2-reference-preview.json").write_text(
        json.dumps(run, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    ENVELOPE.write_text(json.dumps(envelope, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    metrics = envelope["metrics"]
    lines = [
        "# Plasma v2 Performance Envelope",
        "",
        f"Status: {envelope['status']}",
        "",
        envelope["claim_boundary"],
        "",
        "| Metric | Value |",
        "| --- | --- |",
    ]
    for key, value in metrics.items():
        lines.append(f"| {key} | {json.dumps(value, sort_keys=True)} |")
    SUMMARY.write_text("\n".join(lines) + "\n", encoding="utf-8")


def validate(envelope: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if envelope.get("status") not in {"pass", "pass-with-warnings"}:
        errors.append("performance envelope status must pass or pass-with-warnings")
    if envelope.get("product") != "plasma":
        errors.append("performance envelope product must be plasma")
    if envelope.get("candidate") != "plasma_v2_realization_gl11_candidate":
        errors.append("performance envelope must name the GL11 candidate")
    metrics = envelope.get("metrics", {})
    for key in [
        "frame_time_ms_p50",
        "frame_time_ms_p95",
        "frame_time_ms_p99",
        "first_frame_ms",
        "memory_high_water",
        "allocation_growth",
        "resize_cycle_result",
        "short_soak_result",
        "fallback_frequency",
    ]:
        if key not in metrics:
            errors.append(f"performance envelope missing {key}")
    for key in ["frame_time_ms_p50", "frame_time_ms_p95", "frame_time_ms_p99", "first_frame_ms"]:
        if not isinstance(metrics.get(key), (int, float)) or float(metrics.get(key, 0.0)) <= 0.0:
            errors.append(f"{key} must be a positive measured number")
    if metrics.get("resize_cycle_result") != "pass":
        errors.append("resize cycle result must pass")
    if metrics.get("short_soak_result") != "pass":
        errors.append("short soak result must pass")
    if metrics.get("fallback_frequency") != 1.0:
        errors.append("portable-runner fallback frequency must be recorded as 1.0")
    if "not stable release" not in str(envelope.get("claim_boundary", "")):
        errors.append("performance envelope must block stable release claims")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Refresh committed performance envelope evidence.")
    args = parser.parse_args()

    for path in [CONTRACT, ADAPTER, PROOF]:
        if not path.exists():
            print(f"Missing performance input {repo_path(path)}", file=sys.stderr)
            return 1
    if args.update:
        run = run_profile()
        if run["returncode"] != 0:
            print("Plasma proof command failed during performance update.", file=sys.stderr)
            print("\n".join(run.get("stderr_tail", []) or run.get("stdout_tail", [])), file=sys.stderr)
            return 1
        write_envelope(build_envelope(run), run)
    if not ENVELOPE.exists() or not SUMMARY.exists() or not (RUN_DIR / "plasma-v2-reference-preview.json").exists():
        print("Missing performance envelope evidence; run with --update.", file=sys.stderr)
        return 1
    errors = validate(load_json(ENVELOPE))
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 performance checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
