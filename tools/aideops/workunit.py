"""Inspect the ScreenSave AIDE WorkUnit ledgers."""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
WORK_UNITS_DIR = ROOT / ".aide" / "work_units"
LEDGER = WORK_UNITS_DIR / "index.toml"
REFERENCE_SOURCES = [
    ROOT / "PROJECT_STATE.toml",
    ROOT / "docs" / "roadmap",
]
WORK_UNIT_REF = re.compile(r"(?P<path>\.aide[\\/]+work_units[\\/]+[A-Za-z0-9_.\-]+\.toml)")


def load_ledger(path: pathlib.Path = LEDGER) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def referenced_work_unit_paths() -> list[pathlib.Path]:
    paths: list[pathlib.Path] = []
    for source in REFERENCE_SOURCES:
        if source.is_file():
            candidates = [source]
        elif source.is_dir():
            candidates = list(source.glob("*.md"))
        else:
            candidates = []
        for candidate in candidates:
            try:
                text = candidate.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                text = candidate.read_text(encoding="utf-8-sig")
            for match in WORK_UNIT_REF.finditer(text):
                paths.append(ROOT / match.group("path").replace("\\", "/"))
    return paths


def discovery_roots() -> list[pathlib.Path]:
    paths = [LEDGER]
    paths.extend(sorted(path for path in WORK_UNITS_DIR.glob("*.toml") if path.name != "index.toml"))
    paths.extend(referenced_work_unit_paths())
    observed: set[pathlib.Path] = set()
    unique: list[pathlib.Path] = []
    for path in paths:
        resolved = path.resolve()
        if resolved in observed:
            continue
        observed.add(resolved)
        unique.append(path)
    return unique


def work_units(ledger: dict[str, Any]) -> list[dict[str, Any]]:
    return [item for item in ledger.get("work_units", []) if isinstance(item, dict)]


def discovered_work_units() -> tuple[list[dict[str, Any]], list[str]]:
    units: list[dict[str, Any]] = []
    searched: list[str] = []
    seen: set[str] = set()
    for path in discovery_roots():
        searched.append(repo_path(path))
        if not path.exists():
            continue
        payload = load_ledger(path)
        packet_id = payload.get("ledger_id") or payload.get("program_id") or path.stem
        for item in work_units(payload):
            task_id = str(item.get("id", ""))
            if not task_id or task_id in seen:
                continue
            seen.add(task_id)
            record = dict(item)
            record["packet_ref"] = repo_path(path)
            record["packet_id"] = packet_id
            units.append(record)
    return units, searched


def find_work_unit(task_id: str) -> tuple[dict[str, Any] | None, list[str]]:
    units, searched = discovered_work_units()
    for item in units:
        if item.get("id") == task_id:
            return item, searched
    return None, searched


def missing_task_payload(task_id: str, searched: list[str]) -> dict[str, Any]:
    return {
        "status": "blocked",
        "reason": "workunit_not_found",
        "task_id": task_id,
        "searched_roots": searched,
    }


def command_list(_args: argparse.Namespace) -> int:
    units, searched = discovered_work_units()
    payload = {
        "schema": "screensave.workunit.list.v1",
        "ledger": repo_path(LEDGER),
        "searched_roots": searched,
        "count": len(units),
        "work_units": [
            {
                "id": item.get("id"),
                "packet_ref": item.get("packet_ref"),
                "task_class": item.get("task_class"),
                "agent_mode": item.get("agent_mode"),
                "selected_validation_tier": item.get("selected_validation_tier"),
                "title": item.get("title"),
            }
            for item in units
        ],
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


def command_inspect(args: argparse.Namespace) -> int:
    item, searched = find_work_unit(args.task)
    if item is None:
        print(json.dumps(missing_task_payload(args.task, searched), indent=2, sort_keys=True))
        return 1
    print(json.dumps(item, indent=2, sort_keys=True))
    return 0


def command_status(args: argparse.Namespace) -> int:
    item, searched = find_work_unit(args.task)
    if item is None:
        print(json.dumps(missing_task_payload(args.task, searched), indent=2, sort_keys=True))
        return 1
    payload = {
        "schema": "screensave.workunit.status.v1",
        "status": item.get("status", "active"),
        "task_id": item.get("id"),
        "title": item.get("title"),
        "task_class": item.get("task_class"),
        "selected_validation_tier": item.get("selected_validation_tier"),
        "packet_ref": item.get("packet_ref"),
        "agent_allowed": item.get("agent_allowed"),
        "agent_mode": item.get("agent_mode"),
        "human_review_required": item.get("human_review_required"),
        "claim_boundary": item.get("claim_boundary"),
        "expected_validators": item.get("expected_validators", []),
        "evidence_outputs": item.get("evidence_outputs", []),
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    list_cmd = subparsers.add_parser("list")
    list_cmd.set_defaults(func=command_list)
    inspect_cmd = subparsers.add_parser("inspect")
    inspect_cmd.add_argument("--task", required=True)
    inspect_cmd.set_defaults(func=command_inspect)
    status_cmd = subparsers.add_parser("status")
    status_cmd.add_argument("--task", required=True)
    status_cmd.set_defaults(func=command_status)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
