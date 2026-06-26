"""Operate the ScreenSave deterministic repair queue."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
QUEUE = ROOT / ".aide" / "repairs" / "index.toml"
OUT_ROOT = ROOT / "out" / "aide" / "repairs"


def load_queue() -> dict[str, Any]:
    with QUEUE.open("rb") as handle:
        return tomllib.load(handle)


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def repairs(queue: dict[str, Any]) -> list[dict[str, Any]]:
    return [item for item in queue.get("repairs", []) if isinstance(item, dict)]


def write_json(path: pathlib.Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def command_scan(args: argparse.Namespace) -> int:
    queue = load_queue()
    payload = {
        "schema": "screensave.repair.scan.v1",
        "queue": repo_path(QUEUE),
        "status": "pass",
        "repair_count": len(repairs(queue)),
        "repair_classes": queue.get("repair_classes", []),
        "repairs": repairs(queue),
        "claim_boundary": "Repair scan is report-only; it does not mutate source, merge, promote, release, or certify compatibility.",
    }
    output = pathlib.Path(args.out) if args.out else OUT_ROOT / "scan.json"
    if not output.is_absolute():
        output = ROOT / output
    write_json(output, payload)
    print(json.dumps({"status": "pass", "output": repo_path(output)}, indent=2, sort_keys=True))
    return 0


def command_plan(args: argparse.Namespace) -> int:
    queue = load_queue()
    repair_class = args.repair_class
    if repair_class not in set(queue.get("repair_classes", [])):
        print(f"unknown repair class: {repair_class}", file=sys.stderr)
        return 1
    matching = [item for item in repairs(queue) if item.get("repair_class") == repair_class]
    payload = {
        "schema": "screensave.repair.plan.v1",
        "status": "pass",
        "repair_class": repair_class,
        "matching_count": len(matching),
        "tasks": matching,
        "claim_boundary": "Repair plans are advisory until a human or assistant applies a bounded code change and validates it.",
    }
    output = OUT_ROOT / f"plan-{repair_class}.json"
    write_json(output, payload)
    print(json.dumps({"status": "pass", "output": repo_path(output)}, indent=2, sort_keys=True))
    return 0


def command_close(args: argparse.Namespace) -> int:
    queue = load_queue()
    task = args.task
    found = [item for item in repairs(queue) if item.get("id") == task]
    if not found:
        print(f"unknown repair task: {task}", file=sys.stderr)
        return 1
    payload = {
        "schema": "screensave.repair.close-request.v1",
        "status": "recorded",
        "task": task,
        "requires_human_commit": True,
        "claim_boundary": "Close requests are receipts only; they do not prove repair success or mutate the queue.",
    }
    output = OUT_ROOT / f"close-{task}.json"
    write_json(output, payload)
    print(json.dumps({"status": "recorded", "output": repo_path(output)}, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    scan = subparsers.add_parser("scan")
    scan.add_argument("--out", default="")
    scan.set_defaults(func=command_scan)
    plan = subparsers.add_parser("plan")
    plan.add_argument("--class", dest="repair_class", required=True)
    plan.set_defaults(func=command_plan)
    close = subparsers.add_parser("close")
    close.add_argument("--task", required=True)
    close.set_defaults(func=command_close)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
