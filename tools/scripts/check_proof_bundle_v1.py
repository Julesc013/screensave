"""Validate the Proof Bundle v1 contract and normalizer."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "proof_bundle_v1.md"
TOOL = ROOT / "tools" / "proofbundle" / "proofbundle.py"
README = ROOT / "tools" / "proofbundle" / "README.md"
NOCTURNE_PROOF = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne" / "proof.json"
OUTPUT = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-nocturne.json"
LIFECYCLE_DIR = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-lifecycle"
LIFECYCLE_RECEIPT = LIFECYCLE_DIR / "lifecycle.json"
PERFORMANCE_RECEIPT = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-performance.json"

REQUIRED_AXES = {
    "execution",
    "capture",
    "comparison",
    "lifecycle",
    "performance",
    "artifact_audit",
    "compatibility",
    "artistic_review",
    "release_promotion",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []

    for path in (CONTRACT, TOOL, README, NOCTURNE_PROOF):
        require(path.exists(), f"Missing Proof Bundle v1 path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        for needle in [
            "Proof Bundle Contract v1",
            "result_axes",
            "`execution`",
            "`capture`",
            "`comparison`",
            "`lifecycle`",
            "`performance`",
            "`artifact_audit`",
            "`compatibility`",
            "`artistic_review`",
            "`release_promotion`",
            "does not certify an operating system",
            "does not mean the saver looks good",
            "lifecycle receipts",
            "performance receipts",
        ]:
            require(needle in text, f"proof_bundle_v1.md is missing expected text: {needle!r}", errors)

    if TOOL.exists():
        tool_text = TOOL.read_text(encoding="utf-8")
        require("proof-bundle-v1" in tool_text, "proofbundle normalizer must emit proof-bundle-v1.", errors)
        require("result_axes" in tool_text, "proofbundle normalizer must emit result axes.", errors)

    if not errors:
        subprocess.check_call(
            [
                sys.executable,
                str(ROOT / "tools" / "sslab" / "sslab.py"),
                "lifecycle",
                "--product",
                "nocturne",
                "--preset",
                "observatory_night",
                "--width",
                "96",
                "--height",
                "54",
                "--resize-width",
                "80",
                "--resize-height",
                "45",
                "--seed",
                "1536",
                "--frames",
                "8",
                "--delta-ms",
                "100",
                "--output-dir",
                str(LIFECYCLE_DIR),
            ],
            cwd=ROOT,
        )
        PERFORMANCE_RECEIPT.parent.mkdir(parents=True, exist_ok=True)
        PERFORMANCE_RECEIPT.write_text(
            json.dumps(
                {
                    "performance_schema": "screensave-proof-performance-sample-v0",
                    "status": "informational",
                    "frame_count": 8,
                    "frame_time_ms": {
                        "p50": 0.0,
                        "p95": 0.0,
                        "p99": 0.0,
                    },
                    "soak": {
                        "class": "not-run",
                    },
                    "limits": {
                        "classification": "validator fixture only",
                    },
                },
                indent=2,
                sort_keys=True,
            )
            + "\n",
            encoding="utf-8",
        )
        subprocess.check_call(
            [
                sys.executable,
                str(TOOL),
                "normalize",
                "--proof",
                str(NOCTURNE_PROOF),
                "--lifecycle",
                str(LIFECYCLE_RECEIPT),
                "--performance",
                str(PERFORMANCE_RECEIPT),
                "--output",
                str(OUTPUT),
            ],
            cwd=ROOT,
        )
        bundle = json.loads(OUTPUT.read_text(encoding="utf-8"))
        require(bundle.get("proof_schema") == "proof-bundle-v1", "normalized bundle must use proof-bundle-v1.", errors)
        require(bundle.get("status") == "informational", "normalized bundle must remain informational by default.", errors)
        axes = bundle.get("result_axes", {})
        require(set(axes) == REQUIRED_AXES, "normalized bundle must contain the required result axes.", errors)
        require(axes.get("capture", {}).get("status") == "pass", "capture axis must pass for the committed Nocturne proof.", errors)
        require(axes.get("lifecycle", {}).get("status") == "pass", "lifecycle axis must consume a passing lifecycle receipt.", errors)
        require(
            axes.get("lifecycle", {}).get("step_count") == 8,
            "lifecycle axis must retain lifecycle step evidence.",
            errors,
        )
        require(
            axes.get("performance", {}).get("status") == "informational",
            "performance axis must consume informational profile evidence.",
            errors,
        )
        require(
            axes.get("performance", {}).get("frame_count") == 8,
            "performance axis must retain performance frame-count evidence.",
            errors,
        )
        require(axes.get("compatibility", {}).get("certified") is False, "compatibility axis must not certify OS support.", errors)
        require(
            axes.get("artistic_review", {}).get("status") == "blocked",
            "artistic review must remain blocked without human review.",
            errors,
        )
        require(
            axes.get("release_promotion", {}).get("status") == "blocked",
            "release promotion must remain blocked without approval.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Proof Bundle v1 checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
