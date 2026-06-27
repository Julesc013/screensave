"""Validate proposal-only AIDE agentic receipts."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
POLICY = ROOT / ".aide" / "agentic" / "policy.toml"
RC_POLICY = ROOT / ".aide" / "agentic" / "release-candidate-policy.toml"
PROPOSAL_SCHEMA = ROOT / ".aide" / "agentic" / "proposal.schema.json"
RECEIPT_SCHEMA = ROOT / ".aide" / "agentic" / "session-receipt.schema.json"
EXAMPLE_DIR = ROOT / ".aide" / "agentic" / "examples" / "SS-PLV2-H6"
AGENTIC = ROOT / "tools" / "aideops" / "agentic.py"
CHECK_OUT = ROOT / "out" / "aide" / "agentic" / "check-agentic-receipts"


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_required(schema: dict[str, Any], data: dict[str, Any], label: str, errors: list[str]) -> None:
    for key in schema.get("required", []):
        require(key in data, f"{label} missing required field {key}.", errors)


def check_proposal(path: pathlib.Path, errors: list[str]) -> None:
    proposal = load_json(path)
    schema = load_json(PROPOSAL_SCHEMA)
    check_required(schema, proposal, str(path.relative_to(ROOT)), errors)
    require(proposal.get("agent_mode") == "proposal-only", "proposal must be proposal-only.", errors)
    require(proposal.get("allowed_paths"), "proposal must include allowed paths.", errors)
    require(proposal.get("forbidden_paths"), "proposal must include forbidden paths.", errors)
    require(proposal.get("patch_plan"), "proposal must include patch plan.", errors)
    require(proposal.get("validation_plan"), "proposal must include validation plan.", errors)
    require(proposal.get("expected_evidence"), "proposal must include expected evidence.", errors)
    for action in schema.get("forbidden_actions", []):
        require(action in proposal.get("forbidden_actions", []), f"proposal must forbid {action}.", errors)
    boundary = proposal.get("claim_boundary", "")
    require("no source mutation" in boundary, "proposal boundary must block source mutation.", errors)
    require("release" in boundary, "proposal boundary must block release.", errors)


def check_receipt(path: pathlib.Path, errors: list[str]) -> None:
    receipt = load_json(path)
    schema = load_json(RECEIPT_SCHEMA)
    check_required(schema, receipt, str(path.relative_to(ROOT)), errors)
    require(receipt.get("agent_mode") == "proposal-only", "receipt must be proposal-only.", errors)
    for key in ("source_mutation_by_aide", "automatic_apply", "automatic_merge", "automatic_release"):
        require(receipt.get(key) is False, f"receipt {key} must be false.", errors)
    require(receipt.get("expected_evidence"), "receipt must include expected evidence.", errors)
    require("compatibility certification" in receipt.get("claim_boundary", ""), "receipt boundary must block certification.", errors)


def main() -> int:
    errors: list[str] = []
    for path in (POLICY, RC_POLICY, PROPOSAL_SCHEMA, RECEIPT_SCHEMA, AGENTIC, EXAMPLE_DIR / "proposal.json", EXAMPLE_DIR / "session-receipt.json"):
        require(path.exists(), f"Missing agentic receipt path: {path.relative_to(ROOT)}", errors)

    if RC_POLICY.exists():
        policy = tomllib.loads(RC_POLICY.read_text(encoding="utf-8"))
        require(policy.get("status") == "active", "release-candidate policy must be active.", errors)
        for key in ("source_mutation_by_aide", "automatic_apply", "automatic_merge", "automatic_release", "release_publication", "stable_promotion", "compatibility_certification", "final_artistic_acceptance"):
            require(policy.get(key) is False, f"release-candidate policy {key} must be false.", errors)
        for key in ("release_publication", "stable_promotion", "compatibility_certification", "generic_command_execution", "source_mutation_without_operator"):
            require(policy.get("hard_refusals", {}).get(key) is True, f"release-candidate policy hard_refusals.{key} must be true.", errors)

    if (EXAMPLE_DIR / "proposal.json").exists():
        check_proposal(EXAMPLE_DIR / "proposal.json", errors)
    if (EXAMPLE_DIR / "session-receipt.json").exists():
        check_receipt(EXAMPLE_DIR / "session-receipt.json", errors)

    result = subprocess.run(
        [
            sys.executable,
            str(AGENTIC),
            "propose",
            "--task",
            "SS-PLV2-H-CHECK",
            "--objective",
            "Validate release-candidate proposal receipts",
            "--allowed-path",
            ".aide/agentic/**",
            "--forbidden-path",
            "releases/**",
            "--evidence",
            ".aide/agentic/release-candidate-policy.toml",
            "--out",
            str(CHECK_OUT),
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(result.returncode == 0, f"agentic propose failed: {result.stderr}", errors)
    if (CHECK_OUT / "proposal.json").exists():
        check_proposal(CHECK_OUT / "proposal.json", errors)
    else:
        errors.append("agentic propose did not write proposal.json.")
    if (CHECK_OUT / "session-receipt.json").exists():
        check_receipt(CHECK_OUT / "session-receipt.json", errors)
    else:
        errors.append("agentic propose did not write session-receipt.json.")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("AIDE agentic receipt checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
