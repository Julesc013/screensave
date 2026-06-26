"""Proposal-only AIDE agentic session helper for ScreenSave."""

from __future__ import annotations

import argparse
import datetime as _dt
import json
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
POLICY = ROOT / ".aide" / "agentic" / "policy.toml"

ALLOWED_ACTIONS = [
    "compile_task_context",
    "generate_implementation_plan",
    "generate_patch_proposal",
    "generate_validation_plan",
    "generate_repair_proposal",
    "generate_review_summary",
    "record_proposal_receipt",
    "invoke_fixed_project_checks",
]

FORBIDDEN_ACTIONS = [
    "direct_source_mutation",
    "automatic_apply",
    "automatic_merge",
    "automatic_release",
    "worker_self_approval",
    "compatibility_certification",
    "final_artistic_acceptance",
]

CLAIM_BOUNDARY = (
    "Proposal-only AIDE session; no source mutation, automatic apply, merge, "
    "release, compatibility certification, or final artistic acceptance."
)


def write_json(path: pathlib.Path, data: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def propose(args: argparse.Namespace) -> int:
    out_dir = pathlib.Path(args.out)
    if not out_dir.is_absolute():
        out_dir = ROOT / out_dir
    created_at = _dt.datetime.now(_dt.UTC).replace(microsecond=0).isoformat()

    proposal = {
        "schema": "screensave.aide.agentic.proposal.v1",
        "task": args.task,
        "objective": args.objective,
        "agent_mode": "proposal-only",
        "policy_ref": str(POLICY.relative_to(ROOT)).replace("\\", "/"),
        "allowed_actions": ALLOWED_ACTIONS,
        "forbidden_actions": FORBIDDEN_ACTIONS,
        "implementation_plan": [
            "Inspect the WorkUnit and allowed paths.",
            "Prepare a minimal patch proposal inside the declared scope.",
            "List validators required before commit.",
            "Record evidence refs without promotion claims.",
        ],
        "validation_plan": [
            "py -3 tools/scripts/check_project_state.py --summary",
            "py -3 tools/scripts/check_docs_basics.py",
            "git diff --check",
        ],
        "patch_proposal": "OPERATOR_OR_ASSISTANT_TO_AUTHOR",
        "claim_boundary": CLAIM_BOUNDARY,
    }
    proposal_path = out_dir / "proposal.json"
    write_json(proposal_path, proposal)

    receipt = {
        "schema": "screensave.aide.agentic.session-receipt.v1",
        "task": args.task,
        "objective": args.objective,
        "proposal_ref": str(proposal_path.relative_to(ROOT)).replace("\\", "/"),
        "created_at_utc": created_at,
        "source_mutation_by_aide": False,
        "automatic_apply": False,
        "automatic_merge": False,
        "automatic_release": False,
        "claim_boundary": CLAIM_BOUNDARY,
    }
    receipt_path = out_dir / "session-receipt.json"
    write_json(receipt_path, receipt)
    print(str(receipt_path.relative_to(ROOT)).replace("\\", "/"))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    propose_parser = subparsers.add_parser("propose", help="write a proposal-only agentic receipt")
    propose_parser.add_argument("--task", required=True)
    propose_parser.add_argument("--objective", required=True)
    propose_parser.add_argument("--out", required=True)
    propose_parser.set_defaults(func=propose)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
