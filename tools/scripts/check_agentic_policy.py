"""Validate the ScreenSave AIDE proposal-only agentic policy."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
POLICY = ROOT / ".aide" / "agentic" / "policy.toml"
PROPOSAL_SCHEMA = ROOT / ".aide" / "agentic" / "proposal.schema.json"
RECEIPT_SCHEMA = ROOT / ".aide" / "agentic" / "session-receipt.schema.json"
README = ROOT / ".aide" / "agentic" / "README.md"
AGENTIC = ROOT / "tools" / "aideops" / "agentic.py"
CHECK_OUT = ROOT / "out" / "aide" / "agentic" / "check-agentic-policy"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    errors: list[str] = []
    for path in (POLICY, PROPOSAL_SCHEMA, RECEIPT_SCHEMA, README, AGENTIC):
        require(path.exists(), f"Missing agentic policy path {path.relative_to(ROOT)}.", errors)

    if not errors:
        policy = tomllib.loads(POLICY.read_text(encoding="utf-8"))
        require(policy.get("status") == "active", "agentic policy must be active.", errors)
        require(policy.get("default_agent_mode") == "proposal-only", "agentic policy must default to proposal-only.", errors)
        for key in (
            "source_mutation_by_aide",
            "automatic_apply",
            "automatic_merge",
            "automatic_release",
            "worker_self_approval",
            "compatibility_certification",
            "final_artistic_acceptance",
        ):
            require(policy.get(key) is False, f"agentic policy {key} must be false.", errors)
        for key in (
            "direct_source_mutation",
            "automatic_apply",
            "automatic_merge",
            "automatic_release",
            "compatibility_certification",
            "final_artistic_acceptance",
        ):
            require(policy.get("forbidden", {}).get(key) is True, f"agentic policy forbidden.{key} must be true.", errors)
        readme = README.read_text(encoding="utf-8")
        for phrase in (
            "proposal-only",
            "direct AIDE source mutation",
            "automatic release",
            "compatibility certification",
            "final artistic acceptance",
        ):
            require(phrase in readme, f"agentic README is missing {phrase!r}.", errors)

        result = subprocess.run(
            [
                sys.executable,
                str(AGENTIC),
                "propose",
                "--task",
                "SS-G-CHECK",
                "--objective",
                "Validate proposal-only policy",
                "--out",
                str(CHECK_OUT),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        require(result.returncode == 0, "agentic propose command must pass.", errors)
        proposal = CHECK_OUT / "proposal.json"
        receipt = CHECK_OUT / "session-receipt.json"
        require(proposal.exists(), "agentic propose must write proposal.json.", errors)
        require(receipt.exists(), "agentic propose must write session-receipt.json.", errors)
        if proposal.exists() and receipt.exists():
            proposal_data = load_json(proposal)
            receipt_data = load_json(receipt)
            require(proposal_data.get("agent_mode") == "proposal-only", "proposal must use proposal-only mode.", errors)
            require("direct_source_mutation" in proposal_data.get("forbidden_actions", []), "proposal must forbid direct source mutation.", errors)
            require(receipt_data.get("source_mutation_by_aide") is False, "receipt must record no AIDE source mutation.", errors)
            require(receipt_data.get("automatic_release") is False, "receipt must record no automatic release.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE agentic policy checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
