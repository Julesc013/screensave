"""Validate the Plasma v2 AIDE review evidence ledger and exporter."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "evidence" / "plasma-v2-review.toml"
EXPORTER = ROOT / "tools" / "aide_bridge" / "export_plasma_review.py"
OUTPUT = ROOT / "out" / "aide" / "evidence" / "plasma-v2-review-packet.json"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    errors: list[str] = []
    for path in [LEDGER, EXPORTER]:
        require(path.exists(), f"Missing Plasma AIDE review path: {path.relative_to(ROOT)}", errors)

    if not errors:
        ledger = load_toml(LEDGER)
        claims = ledger.get("claims", {})
        refs = ledger.get("refs", {})
        require(ledger.get("schema_version") == 1, "Plasma review ledger schema_version must be 1.", errors)
        require(ledger.get("runtime_dependency_allowed") is False, "Plasma review ledger must not allow runtime dependency.", errors)
        require(ledger.get("network_calls") is False, "Plasma review ledger must remain network-free.", errors)
        require(ledger.get("provider_or_model_calls") is False, "Plasma review ledger must remain model-free.", errors)
        require(claims.get("stable_release_promotion") == "blocked", "Stable release promotion must remain blocked.", errors)
        require(claims.get("compatibility_certification") == "not-claimed", "Compatibility certification must remain not-claimed.", errors)
        require(claims.get("aide_runtime_dependency") is False, "AIDE runtime dependency claim must be false.", errors)
        for key, ref in refs.items():
            require((ROOT / str(ref)).exists(), f"Plasma review ledger ref does not exist: {key}={ref}", errors)

        result = subprocess.run(
            [
                sys.executable,
                str(EXPORTER),
                "--ledger",
                str(LEDGER),
                "--output",
                str(OUTPUT),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        require(result.returncode == 0, f"Plasma review exporter failed: {result.stderr or result.stdout}", errors)
        require(OUTPUT.exists(), "Plasma review exporter must write a packet.", errors)

    if OUTPUT.exists():
        packet = load_json(OUTPUT)
        packet_claims = packet.get("claims", {})
        facts = packet.get("review_facts", {})
        require(packet.get("schema_version") == "screensave.aide-plasma-review-evidence.v0", "Plasma review packet schema mismatch.", errors)
        require(packet.get("kind") == "EvidencePacket", "Plasma review packet kind must be EvidencePacket.", errors)
        require(packet.get("subject", {}).get("product") == "plasma", "Plasma review packet subject must be plasma.", errors)
        require(packet_claims.get("plasma_v2_reference_execution", {}).get("status") == "supported", "Reference execution claim must be supported.", errors)
        require(packet_claims.get("pack_compilation", {}).get("status") == "supported", "Pack compilation claim must be supported.", errors)
        require(packet_claims.get("candidate_generation", {}).get("status") == "supported", "Candidate generation claim must be supported.", errors)
        require(packet_claims.get("control_influence", {}).get("status") == "supported", "Control influence claim must be supported.", errors)
        require(packet_claims.get("material_distinctness", {}).get("status") == "supported", "Material distinctness claim must be supported.", errors)
        require(packet_claims.get("visual_review", {}).get("status") == "recorded-protocol", "Visual review must be recorded as protocol.", errors)
        require(packet_claims.get("artistic_acceptance", {}).get("status") == "not-yet-final", "Artistic acceptance must remain not-yet-final.", errors)
        require(packet_claims.get("stable_release_promotion", {}).get("status") == "blocked", "Stable release promotion must remain blocked.", errors)
        require(packet_claims.get("compatibility_certification", {}).get("status") == "not-claimed", "Compatibility certification must remain not-claimed.", errors)
        require(packet_claims.get("aide_runtime_dependency", {}).get("value") is False, "AIDE runtime dependency value must be false.", errors)
        require(facts.get("candidate_count") == 3, "Review packet must retain three VisualIntent candidates.", errors)
        require(facts.get("material_count") == 5, "Review packet must retain five materials.", errors)
        require(facts.get("model_calls") is False, "Review packet must record no model calls.", errors)
        require(facts.get("network_calls") is False, "Review packet must record no network calls.", errors)
        require(facts.get("automatic_promotion") is False, "Review packet must record no automatic promotion.", errors)
        require("does not decide final artistic acceptance" in packet.get("claim_boundary", ""), "Review packet must preserve product authority boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma AIDE review checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
