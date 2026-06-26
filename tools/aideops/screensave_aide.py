"""ScreenSave-local operator for imported AIDE Lite commands."""

from __future__ import annotations

import argparse
import datetime as _dt
import hashlib
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
AIDE_LITE = ROOT / ".aide" / "scripts" / "aide_lite.py"
AIDE_LOCK = ROOT / ".aide" / "aide_lite.lock.toml"
OPERATION_ROOT = ROOT / "out" / "aide" / "operations"
WORK_UNIT_FILES = [
    ROOT / ".aide" / "work_units" / "paw-cx-portable-v2.toml",
    ROOT / ".aide" / "work_units" / "index.toml",
    ROOT / ".aide" / "work_units" / "paw-c1-portable-v2-runtime-equivalence.toml",
    ROOT / ".aide" / "work_units" / "plasma-v2-reference-slice.toml",
    ROOT / ".aide" / "work_units" / "plasma-v2-visual-review-and-packc.toml",
    ROOT / ".aide" / "work_units" / "plasma-v2-accelerated-stable-candidate.toml",
]
PORTABLE_EVAL_TASKS = [
    "compact-task-packet-required-sections",
    "context-packet-no-full-repo-dump",
    "verifier-detects-bad-evidence",
    "token-ledger-budget-check",
    "install_no_apply_golden",
]


def repo_path(path: pathlib.Path) -> str:
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def source_revision() -> dict[str, Any]:
    status = git_text(["status", "--short"])
    return {
        "commit": git_text(["rev-parse", "HEAD"]),
        "branch": git_text(["branch", "--show-current"]),
        "dirty": bool(status),
        "status_short": status.splitlines(),
    }


def changed_file_summary(status_short: list[str]) -> dict[str, Any]:
    summary = {
        "total": 0,
        "added": 0,
        "modified": 0,
        "deleted": 0,
        "renamed": 0,
        "untracked": 0,
        "files": [],
    }
    files: list[dict[str, str]] = []
    for line in status_short:
        if len(line) < 4:
            continue
        code = line[:2]
        path = line[3:]
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        files.append({"status": code, "path": path})
        if "A" in code:
            summary["added"] += 1
        if "M" in code:
            summary["modified"] += 1
        if "D" in code:
            summary["deleted"] += 1
        if "R" in code:
            summary["renamed"] += 1
        if code == "??":
            summary["untracked"] += 1
    summary["total"] = len(files)
    summary["files"] = files[:200]
    return summary


def work_unit_records() -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for path in WORK_UNIT_FILES:
        if not path.exists():
            continue
        payload = load_toml(path)
        for item in payload.get("work_units", []):
            if isinstance(item, dict):
                record = dict(item)
                record["packet_ref"] = repo_path(path)
                records.append(record)
    return records


def validation_tier_selection(task_id: str) -> dict[str, Any]:
    for item in work_unit_records():
        if item.get("id") == task_id:
            return {
                "task": task_id,
                "selected_validation_tier": item.get("selected_validation_tier", "unknown"),
                "packet_ref": item.get("packet_ref", ""),
                "status": item.get("status", "unknown"),
            }
    return {
        "task": task_id,
        "selected_validation_tier": "unknown",
        "packet_ref": "",
        "status": "not-found",
    }


def lock_payload() -> dict[str, Any]:
    lock = load_toml(AIDE_LOCK) if AIDE_LOCK.exists() else {}
    return {
        "aide_source_pin": lock.get("source", {}).get("pinned_commit", "unknown"),
        "portable_pack_digest": lock.get("pack", {}).get("manifest_sha256", "unknown"),
        "lock_ref": repo_path(AIDE_LOCK),
        "lock_sha256": sha256_file(AIDE_LOCK) if AIDE_LOCK.exists() else "missing",
    }


def operation_id(command: str, explicit: str | None) -> str:
    if explicit:
        return explicit
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    return f"{stamp}-{command}"


def aide_cmd(*args: str) -> list[str]:
    return [sys.executable, str(AIDE_LITE), *args]


def portable_eval_commands() -> list[list[str]]:
    return [aide_cmd("eval", "run", "--task", task_id) for task_id in PORTABLE_EVAL_TASKS]


def command_plan(command: str, args: argparse.Namespace) -> list[list[str]]:
    task_text = getattr(args, "objective", "") or "Bootstrap ScreenSave PAW-CX portable-v2 development"
    task_id = getattr(args, "task", "") or "SS-AXR-01"
    if command == "status":
        return []
    if command == "bootstrap":
        return [
            aide_cmd("doctor"),
            aide_cmd("validate"),
            aide_cmd("snapshot"),
            aide_cmd("index"),
            aide_cmd("context"),
            aide_cmd("pack", "--task", task_text),
            aide_cmd(
                "verify",
                "--task-packet",
                ".aide/context/latest-task-packet.md",
                "--write-report",
                ".aide/verification/latest-verification-report.md",
            ),
            aide_cmd("review-pack"),
            aide_cmd("ledger", "scan"),
            aide_cmd("ledger", "report"),
            *portable_eval_commands(),
            aide_cmd("eval", "report"),
            aide_cmd("ledger", "scan"),
            aide_cmd("ledger", "report"),
            aide_cmd("outcome", "report"),
            aide_cmd("optimize", "suggest"),
            aide_cmd("validate"),
        ]
    if command == "preflight":
        return [
            aide_cmd("snapshot"),
            aide_cmd("index"),
            aide_cmd("context"),
            aide_cmd("pack", "--task", f"{task_id}: {task_text}"),
            aide_cmd(
                "verify",
                "--task-packet",
                ".aide/context/latest-task-packet.md",
                "--write-report",
                ".aide/verification/latest-verification-report.md",
            ),
        ]
    if command == "postflight":
        return [
            aide_cmd("verify", "--changed-files", "--write-report", ".aide/verification/latest-verification-report.md"),
            aide_cmd("review-pack"),
            aide_cmd("ledger", "scan"),
            aide_cmd("ledger", "report"),
            *portable_eval_commands(),
            aide_cmd("eval", "report"),
            aide_cmd("outcome", "report"),
            aide_cmd("optimize", "suggest"),
            aide_cmd("validate"),
        ]
    if command == "full-check":
        return [
            aide_cmd("doctor"),
            aide_cmd("validate"),
            aide_cmd("ledger", "scan"),
            aide_cmd("ledger", "report"),
            *portable_eval_commands(),
            aide_cmd("eval", "report"),
            aide_cmd("outcome", "report"),
            aide_cmd("optimize", "suggest"),
            aide_cmd("test"),
            aide_cmd("validate"),
        ]
    raise ValueError(f"unknown operation command: {command}")


def generated_refs() -> list[str]:
    refs = [
        ".aide/context/latest-task-packet.md",
        ".aide/context/latest-context-packet.md",
        ".aide/verification/latest-verification-report.md",
        ".aide/context/latest-review-packet.md",
        ".aide/reports/token-ledger.jsonl",
        ".aide/reports/token-savings-summary.md",
        ".aide/evals/runs/latest-golden-tasks.json",
        ".aide/evals/runs/latest-golden-tasks.md",
        ".aide/controller/outcome-ledger.jsonl",
        ".aide/controller/latest-outcome-report.md",
        ".aide/controller/latest-recommendations.md",
    ]
    return [ref for ref in refs if (ROOT / ref).exists()]


def run_sequence(commands: list[list[str]], dry_run: bool, continue_on_error: bool) -> tuple[list[dict[str, Any]], list[str]]:
    results: list[dict[str, Any]] = []
    blocked: list[str] = []
    for command in commands:
        if dry_run:
            results.append(
                {
                    "argv": command,
                    "returncode": None,
                    "status": "planned",
                    "stdout_tail": "",
                    "stderr_tail": "",
                }
            )
            continue
        result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        status = "pass" if result.returncode == 0 else "blocked"
        results.append(
            {
                "argv": command,
                "returncode": result.returncode,
                "status": status,
                "stdout_tail": result.stdout.strip().splitlines()[-20:],
                "stderr_tail": result.stderr.strip().splitlines()[-20:],
            }
        )
        if result.returncode != 0:
            blocked.append(" ".join(command))
            if not continue_on_error:
                break
    return results, blocked


def write_receipt(command: str, args: argparse.Namespace) -> tuple[pathlib.Path, list[str]]:
    op_id = operation_id(command, getattr(args, "operation_id", None))
    output_dir = OPERATION_ROOT / op_id
    output_dir.mkdir(parents=True, exist_ok=True)
    before = source_revision()
    commands = command_plan(command, args)
    results, blocked = run_sequence(commands, bool(getattr(args, "dry_run", False)), command == "bootstrap")
    after = source_revision()
    task_id = getattr(args, "task", "")
    receipt = {
        "schema_version": "screensave.aide-operation-receipt.v0",
        "receipt_kind": f"{command}-receipt",
        "operation_id": op_id,
        "operation": command,
        "task": task_id,
        "objective": getattr(args, "objective", ""),
        "dry_run": bool(getattr(args, "dry_run", False)),
        "validation_tier_selection": validation_tier_selection(task_id) if task_id else {},
        "aide": lock_payload(),
        "screensave_source_revision_before": before,
        "screensave_source_revision_after": after,
        "changed_file_summary_before": changed_file_summary(before.get("status_short", [])),
        "changed_file_summary_after": changed_file_summary(after.get("status_short", [])),
        "commands_invoked": results,
        "generated_artifact_refs": generated_refs(),
        "warnings": [
            "AIDE DistributionManifest v1 remains unaccepted for automatic update.",
            "Generated reports are local operation outputs, not product truth.",
        ],
        "blocked_stages": blocked,
        "blocked_step_list": blocked,
        "provider_calls": False,
        "model_calls": False,
        "network_calls": False,
        "source_mutation_outside_admitted_paths": False,
        "claim_boundary": "AIDE Lite coordination only; no source editing worker, no merge, no release promotion, no compatibility certification.",
    }
    receipt_path = output_dir / "operation-receipt.json"
    receipt_path.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps({"status": "blocked" if blocked else "pass", "receipt": repo_path(receipt_path)}, indent=2, sort_keys=True))
    return receipt_path, blocked


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--operation-id", help="Stable operation id for receipt output.")
    parser.add_argument("--dry-run", action="store_true", help="Write a receipt with planned commands without invoking them.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("status", help="Write a status-only operation receipt.")
    subparsers.add_parser("bootstrap", help="Run the two-pass ScreenSave AIDE Lite bootstrap.")

    preflight = subparsers.add_parser("preflight", help="Prepare compact task context for a WorkUnit.")
    preflight.add_argument("--task", required=True)
    preflight.add_argument("--objective", required=True)

    postflight = subparsers.add_parser("postflight", help="Run post-implementation AIDE Lite review and ledger steps.")
    postflight.add_argument("--task", required=True)

    subparsers.add_parser("full-check", help="Run AIDE Lite health, ledger, golden tasks, outcome, and self tests.")
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    _receipt_path, blocked = write_receipt(args.command, args)
    return 1 if blocked else 0


if __name__ == "__main__":
    raise SystemExit(main())
