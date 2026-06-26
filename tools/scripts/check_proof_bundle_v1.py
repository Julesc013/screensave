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
RICOCHET_PROOF_DIR = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-ricochet-proof"
RICOCHET_PROFILE_DIR = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-ricochet-profile"
RICOCHET_BUNDLE = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-ricochet.json"
PORTABLE_V2_EQUIVALENCE = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-portable-v2-equivalence.json"
PORTABLE_V2_EQUIVALENCE_WORK = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-portable-v2-equivalence-captures"

REQUIRED_AXES = {
    "execution",
    "capture",
    "comparison",
    "lifecycle",
    "performance",
    "portable_v2_equivalence",
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
            "`portable_v2_equivalence`",
            "`artifact_audit`",
            "`compatibility`",
            "`artistic_review`",
            "`release_promotion`",
            "does not certify an operating system",
            "does not mean the saver looks good",
            "lifecycle receipts",
            "performance receipts",
            "portable v2 equivalence receipts",
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
        require(
            axes.get("portable_v2_equivalence", {}).get("status") == "informational",
            "portable v2 equivalence axis must be present even without supplied equivalence evidence.",
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

        subprocess.check_call(
            [
                sys.executable,
                str(ROOT / "tools" / "sslab" / "sslab.py"),
                "proof",
                "--profile",
                "ricochet.reference.v1",
                "--output-dir",
                str(RICOCHET_PROOF_DIR),
            ],
            cwd=ROOT,
        )
        subprocess.check_call(
            [
                sys.executable,
                str(ROOT / "tools" / "sslab" / "sslab.py"),
                "profile",
                "--product",
                "ricochet",
                "--preset",
                "classic_clean",
                "--width",
                "128",
                "--height",
                "72",
                "--seed",
                "2048",
                "--frames",
                "32",
                "--delta-ms",
                "100",
                "--iterations",
                "2",
                "--short-soak-cycles",
                "2",
                "--output-dir",
                str(RICOCHET_PROFILE_DIR),
            ],
            cwd=ROOT,
        )
        subprocess.check_call(
            [
                sys.executable,
                str(ROOT / "tools" / "sslab" / "sslab_equivalence.py"),
                "--output",
                str(PORTABLE_V2_EQUIVALENCE),
                "--work-root",
                str(PORTABLE_V2_EQUIVALENCE_WORK),
            ],
            cwd=ROOT,
        )
        subprocess.check_call(
            [
                sys.executable,
                str(TOOL),
                "normalize",
                "--proof",
                str(RICOCHET_PROOF_DIR / "profile-proof.json"),
                "--performance",
                str(RICOCHET_PROFILE_DIR / "profile.json"),
                "--portable-v2-equivalence",
                str(PORTABLE_V2_EQUIVALENCE),
                "--output",
                str(RICOCHET_BUNDLE),
            ],
            cwd=ROOT,
        )
        ricochet_bundle = json.loads(RICOCHET_BUNDLE.read_text(encoding="utf-8"))
        ricochet_axes = ricochet_bundle.get("result_axes", {})
        require(ricochet_bundle.get("subject", {}).get("product") == "ricochet", "Ricochet bundle must record the product key.", errors)
        require(ricochet_bundle.get("subject", {}).get("profile") == "ricochet.reference.v1", "Ricochet bundle must record the proof profile.", errors)
        require(ricochet_axes.get("capture", {}).get("status") == "pass", "Ricochet bundle capture axis must pass from profile captures.", errors)
        require(ricochet_axes.get("capture", {}).get("capture_count") == 4, "Ricochet bundle must retain four profile captures.", errors)
        require(ricochet_axes.get("comparison", {}).get("status") == "pass", "Ricochet bundle comparison axis must pass from profile proof.", errors)
        require(ricochet_axes.get("lifecycle", {}).get("status") == "pass", "Ricochet bundle lifecycle axis must pass from profile proof.", errors)
        require(
            ricochet_axes.get("lifecycle", {}).get("create_destroy_cycles") == 32,
            "Ricochet bundle lifecycle axis must retain 32 create/destroy cycles.",
            errors,
        )
        require(
            ricochet_axes.get("performance", {}).get("status") == "informational",
            "Ricochet bundle performance axis must consume informational profile evidence.",
            errors,
        )
        require(
            ricochet_axes.get("performance", {}).get("soak", {}).get("status") == "pass",
            "Ricochet bundle performance axis must retain short-soak status.",
            errors,
        )
        portable_axis = ricochet_axes.get("portable_v2_equivalence", {})
        require(portable_axis.get("status") == "pass", "portable v2 equivalence axis must pass when receipt is supplied.", errors)
        require(
            portable_axis.get("products") == ["nocturne", "ricochet"],
            "portable v2 equivalence axis must identify Nocturne and Ricochet products.",
            errors,
        )
        require(
            portable_axis.get("profiles") == ["nocturne.reference.v0", "ricochet.reference.v1"],
            "portable v2 equivalence axis must identify the canary proof profiles.",
            errors,
        )
        require(
            portable_axis.get("claim_boundary") == "Named canary v1/v2 deterministic equivalence only.",
            "portable v2 equivalence axis must keep the named-canary claim boundary.",
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
