"""Validate the ScreenSave AIDE evidence index."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
INDEX = ROOT / ".aide" / "evidence" / "index.toml"
INDEXER = ROOT / "tools" / "aide_bridge" / "index_evidence.py"
SUMMARY = ROOT / "out" / "aide" / "evidence" / "index-summary.json"

REQUIRED_IDS = {
    "nocturne-v1-proof",
    "nocturne-v2-proof",
    "ricochet-v1-proof",
    "ricochet-v2-proof",
    "plasma-v2-proof",
    "plasma-v2-proof-bundle",
    "plasma-v2-materials",
    "plasma-v2-packc",
    "plasma-v2-evidence-packet",
    "plasma-v2-proof-matrix",
    "plasma-v2-visualintent-candidates",
    "plasma-v2-review-ledger",
    "plasma-v2-review-evidence-packet",
    "plasma-v2-packc-v1-candidate",
    "plasma-v2-acceleration-candidate",
    "plasma-v2-performance-envelope",
    "plasma-v2-stable-candidate-review",
    "plasma-v2-stable-candidate-evidence-packet",
    "portable-v2-equivalence-proof",
    "windows-current-artifact-manifests",
    "windows-current-pe-audit-json",
    "proof-bundle-v1-portable-v2",
    "aide-evidence-packets",
    "plasma-v2-instrument-repair",
    "plasma-v2-visualintent-spec-reduction",
    "plasma-v2-visualintent-proof-summary",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [INDEX, INDEXER]:
        require(path.exists(), f"Missing AIDE evidence index path: {path.relative_to(ROOT)}", errors)

    if not errors:
        result = subprocess.run(
            [sys.executable, str(INDEXER), "--check", "--output", str(SUMMARY)],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        require(result.returncode == 0, f"index_evidence.py failed: {result.stderr}", errors)
        require(SUMMARY.exists(), "index_evidence.py must write index-summary.json.", errors)

    if SUMMARY.exists():
        summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
        entry_ids = set(summary.get("entry_ids", []))
        require(summary.get("status") == "pass", "evidence index summary must pass.", errors)
        require(REQUIRED_IDS <= entry_ids, "evidence index summary is missing required entries.", errors)
        require(summary.get("network_calls") is False, "evidence index must remain network-free.", errors)
        require(summary.get("provider_or_model_calls") is False, "evidence index must remain model-free.", errors)
        require(summary.get("runtime_dependency_allowed") is False, "AIDE evidence index must not become a runtime dependency.", errors)
        require(
            "ScreenSave proof bundles and validators remain authoritative" in summary.get("truth_boundary", ""),
            "evidence index must preserve the ScreenSave proof authority boundary.",
            errors,
        )

    if INDEX.exists():
        text = INDEX.read_text(encoding="utf-8")
        for needle in [
            "portable-v2-equivalence-proof",
            "plasma-v2-proof",
            "plasma-v2-materials",
            "plasma-v2-packc",
            "plasma-v2-review-ledger",
            "plasma-v2-review-evidence-packet",
            "plasma-v2-acceleration-candidate",
            "plasma-v2-performance-envelope",
            "plasma-v2-stable-candidate-review",
            "plasma-v2-instrument-repair",
            "plasma-v2-visualintent-spec-reduction",
            "plasma-v2-visualintent-proof-summary",
            "check_plasma_v2_visualintent_proof.py",
            "VisualIntent-to-Plasma spec reduction evidence only",
            "VisualIntent proof summary evidence only",
            "compatibility certification",
            "artistic acceptance",
            "release promotion",
            "AIDE indexes references only",
        ]:
            require(needle in text, f"evidence index missing expected boundary text: {needle}", errors)
        for forbidden in ["provider_or_model_calls = true", "network_calls = true", "runtime_dependency_allowed = true"]:
            require(forbidden not in text, f"evidence index must not contain {forbidden}.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE evidence index checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
