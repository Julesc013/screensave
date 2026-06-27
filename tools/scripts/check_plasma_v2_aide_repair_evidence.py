"""Validate AIDE evidence for Plasma v2 instrument repair."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "evidence" / "plasma-v2-instrument-repair.toml"
INDEX = ROOT / ".aide" / "evidence" / "index.toml"
SUMMARY = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "aide-repair-summary.json"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def main() -> int:
    errors: list[str] = []
    for path in (LEDGER, INDEX, SUMMARY):
        require(path.exists(), f"Missing AIDE instrument repair evidence path: {path.relative_to(ROOT)}", errors)
    if not errors:
        ledger = load_toml(LEDGER)
        summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
        index_text = INDEX.read_text(encoding="utf-8")
        require(ledger.get("status") == "active", "AIDE instrument repair ledger must be active.", errors)
        require(ledger.get("network_calls") is False, "AIDE instrument repair ledger must be network-free.", errors)
        require(ledger.get("provider_or_model_calls") is False, "AIDE instrument repair ledger must be model-free.", errors)
        require(ledger.get("runtime_dependency_allowed") is False, "AIDE must not become a runtime dependency.", errors)
        for key in (
            "source_mutation_by_aide",
            "automatic_apply",
            "automatic_merge",
            "automatic_release",
            "stable_promotion",
            "compatibility_certification",
            "final_artistic_acceptance",
        ):
            require(ledger.get(key) is False, f"AIDE instrument repair ledger {key} must be false.", errors)
        claims = ledger.get("claims", {})
        require(claims.get("agentic_session") == "proposal-only", "AIDE agentic session must remain proposal-only.", errors)
        require(claims.get("remaining_blocker") == "SS-PLV2-I-REPAIR-001", "AIDE summary must record the remaining stable artistic verdict blocker.", errors)
        require(summary.get("status") == "pass", "AIDE repair summary status must pass.", errors)
        require(summary.get("agentic", {}).get("agent_mode") == "proposal-only", "AIDE repair summary must record proposal-only mode.", errors)
        require(summary.get("agentic", {}).get("source_mutation_by_aide") is False, "AIDE repair summary must record no source mutation.", errors)
        require(summary.get("plans", {}).get("release_readiness_gap", {}).get("matching_count") == 1, "AIDE repair summary must record one release_readiness_gap blocker.", errors)
        require(summary.get("plans", {}).get("proof_baseline_drift", {}).get("matching_count") == 0, "AIDE repair summary must record no proof baseline drift repairs.", errors)
        require(summary.get("plans", {}).get("workbench_surface_drift", {}).get("matching_count") == 0, "AIDE repair summary must record no Workbench surface drift repairs.", errors)
        require("plasma-v2-instrument-repair" in index_text, "AIDE evidence index must track the instrument repair ledger.", errors)
        require("human artistic acceptance remain authoritative" in index_text, "AIDE evidence index must preserve the human acceptance boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 AIDE repair evidence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
