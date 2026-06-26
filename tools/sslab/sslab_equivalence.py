"""Run portable v2 equivalence checks for the fixed canary profiles."""

from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import sys
import tempfile
from typing import Any

THIS_DIR = pathlib.Path(__file__).resolve().parent
if str(THIS_DIR) not in sys.path:
    sys.path.insert(0, str(THIS_DIR))

from build_support import build_runner

ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_OUTPUT = ROOT / "out" / "proof" / "portable-v2-equivalence" / "equivalence.json"
DEFAULT_WORK_ROOT = ROOT / "out" / "proof" / "portable-v2-equivalence" / "captures"
PROFILES = ("nocturne.reference.v0", "ricochet.reference.v1")


def display_path(path: pathlib.Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT))
    except ValueError:
        return str(path)


def run_runner(
    runner: pathlib.Path,
    profile: str,
    execution_path: str,
    work_root: pathlib.Path,
) -> dict[str, Any]:
    output_root = work_root / profile / execution_path / "captures"
    output_json = work_root / profile / execution_path / "runner-proof.json"
    output_root.mkdir(parents=True, exist_ok=True)
    output_json.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [
            str(runner),
            "proof",
            "--profile",
            profile,
            "--path",
            execution_path,
            "--output-root",
            str(output_root),
            "--output",
            str(output_json),
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(f"{profile} {execution_path} failed: {result.stderr.strip()}")
    return json.loads(output_json.read_text(encoding="utf-8"))


def capture_pairs(result: dict[str, Any]) -> list[dict[str, Any]]:
    pairs: list[dict[str, Any]] = []
    for capture in result.get("captures", []):
        pairs.append(
            {
                "frame": int(capture.get("frame", 0)),
                "status": str(capture.get("status", "")),
                "rgba_sha256": str(capture.get("rgba_sha256", "")),
            }
        )
    return pairs


def compare_profile(profile: str, v1: dict[str, Any], v2: dict[str, Any]) -> dict[str, Any]:
    v1_captures = capture_pairs(v1)
    v2_captures = capture_pairs(v2)
    frame_hash_match = v1_captures == v2_captures
    lifecycle_v1 = v1.get("lifecycle", {})
    lifecycle_v2 = v2.get("lifecycle", {})
    profile_v1 = v1.get("profile_receipt", {})
    profile_v2 = v2.get("profile_receipt", {})
    lifecycle_match = (
        isinstance(lifecycle_v1, dict)
        and isinstance(lifecycle_v2, dict)
        and lifecycle_v1.get("status") == "pass"
        and lifecycle_v2.get("status") == "pass"
        and lifecycle_v1.get("create_destroy_cycles") == lifecycle_v2.get("create_destroy_cycles")
    )
    profile_match = (
        isinstance(profile_v1, dict)
        and isinstance(profile_v2, dict)
        and profile_v1.get("status") == "pass"
        and profile_v2.get("status") == "pass"
        and profile_v1.get("short_soak_frames") == profile_v2.get("short_soak_frames")
    )
    status = "pass" if frame_hash_match and lifecycle_match and profile_match else "fail"
    return {
        "profile": profile,
        "status": status,
        "v1_status": v1.get("status"),
        "v2_status": v2.get("status"),
        "captures": [
            {
                "frame": v1_capture.get("frame"),
                "v1_rgba_sha256": v1_capture.get("rgba_sha256"),
                "v2_rgba_sha256": v2_captures[index].get("rgba_sha256") if index < len(v2_captures) else "",
                "match": index < len(v2_captures) and v1_capture == v2_captures[index],
            }
            for index, v1_capture in enumerate(v1_captures)
        ],
        "frame_hash_match": frame_hash_match,
        "lifecycle_match": lifecycle_match,
        "profile_pass_class_match": profile_match,
        "v1_runner_ref": display_path(pathlib.Path(str(v1.get("runner_ref", "")))) if v1.get("runner_ref") else "",
        "v2_runner_ref": display_path(pathlib.Path(str(v2.get("runner_ref", "")))) if v2.get("runner_ref") else "",
    }


def run_equivalence(output: pathlib.Path, work_root: pathlib.Path) -> dict[str, Any]:
    profile_results: list[dict[str, Any]] = []
    with tempfile.TemporaryDirectory(prefix="sslab-equivalence-") as temp_root:
        runner = build_runner(pathlib.Path(temp_root))
        for profile in PROFILES:
            v1 = run_runner(runner, profile, "v1", work_root)
            v2 = run_runner(runner, profile, "v2", work_root)
            v1["runner_ref"] = str(work_root / profile / "v1" / "runner-proof.json")
            v2["runner_ref"] = str(work_root / profile / "v2" / "runner-proof.json")
            profile_results.append(compare_profile(profile, v1, v2))

    status = "pass" if all(item.get("status") == "pass" for item in profile_results) else "fail"
    receipt = {
        "equivalence_schema": "screensave-portable-v2-equivalence-v0",
        "status": status,
        "claim_boundary": "v1/v2 deterministic equivalence for Nocturne and Ricochet canary proof profiles only",
        "profiles": profile_results,
        "output_ref": display_path(output),
        "work_root": display_path(work_root),
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return receipt


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT))
    parser.add_argument("--work-root", default=str(DEFAULT_WORK_ROOT))
    args = parser.parse_args()
    output = pathlib.Path(args.output)
    work_root = pathlib.Path(args.work_root)
    if not output.is_absolute():
        output = ROOT / output
    if not work_root.is_absolute():
        work_root = ROOT / work_root
    receipt = run_equivalence(output, work_root)
    print(f"portable-v2-equivalence {receipt['status']} {display_path(output)}")
    return 0 if receipt.get("status") == "pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())
