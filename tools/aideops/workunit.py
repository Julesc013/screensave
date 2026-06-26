"""Inspect the ScreenSave AIDE WorkUnit ledger."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "work_units" / "index.toml"


def load_ledger(path: pathlib.Path = LEDGER) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def work_units(ledger: dict[str, Any]) -> list[dict[str, Any]]:
    return [item for item in ledger.get("work_units", []) if isinstance(item, dict)]


def command_list(_args: argparse.Namespace) -> int:
    ledger = load_ledger()
    payload = {
        "schema": "screensave.workunit.list.v1",
        "ledger": repo_path(LEDGER),
        "count": len(work_units(ledger)),
        "work_units": [
            {
                "id": item.get("id"),
                "task_class": item.get("task_class"),
                "agent_mode": item.get("agent_mode"),
                "selected_validation_tier": item.get("selected_validation_tier"),
                "title": item.get("title"),
            }
            for item in work_units(ledger)
        ],
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


def command_inspect(args: argparse.Namespace) -> int:
    ledger = load_ledger()
    for item in work_units(ledger):
        if item.get("id") == args.task:
            print(json.dumps(item, indent=2, sort_keys=True))
            return 0
    print(f"unknown WorkUnit: {args.task}", file=sys.stderr)
    return 1


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    list_cmd = subparsers.add_parser("list")
    list_cmd.set_defaults(func=command_list)
    inspect_cmd = subparsers.add_parser("inspect")
    inspect_cmd.add_argument("--task", required=True)
    inspect_cmd.set_defaults(func=command_inspect)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
