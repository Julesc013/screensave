"""Validate the project-scoped Codex configuration and registered custom agents."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIG_PATH = ROOT / ".codex" / "config.toml"
VSCODE_SETTINGS_PATH = ROOT / ".vscode" / "settings.json"
EXPECTED_MODEL = "gpt-5.5"
EXPECTED_FOLLOW_UP_QUEUE_MODE = "queue"

EXPECTED_AGENT_NAMES = {
    "repo_explorer",
    "repo_architect",
    "implementer",
    "reviewer",
}

APPROVED_SECURITY_PROFILES = {
    ("on-request", "workspace-write"),
    ("never", "danger-full-access"),
}


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict:
    with path.open("r", encoding="utf-8") as handle:
        value = json.load(handle)
    if not isinstance(value, dict):
        raise ValueError(f"Expected {path.relative_to(ROOT)} to contain a JSON object.")
    return value


def main() -> int:
    config = load_toml(CONFIG_PATH)
    errors = []

    if config.get("model") != EXPECTED_MODEL:
        errors.append(f"Expected .codex/config.toml to set model = {EXPECTED_MODEL!r}.")

    try:
        vscode_settings = load_json(VSCODE_SETTINGS_PATH)
    except (OSError, json.JSONDecodeError, ValueError) as exc:
        errors.append(f"Could not read .vscode/settings.json: {exc}")
        vscode_settings = {}

    follow_up_mode = vscode_settings.get("chatgpt.followUpQueueMode")
    if follow_up_mode != EXPECTED_FOLLOW_UP_QUEUE_MODE:
        errors.append(
            "Expected .vscode/settings.json to set "
            f"chatgpt.followUpQueueMode = {EXPECTED_FOLLOW_UP_QUEUE_MODE!r} so "
            "in-progress Codex follow-ups are queued instead of steering or interrupting the current run."
        )

    security_profile = (config.get("approval_policy"), config.get("sandbox_mode"))
    if security_profile not in APPROVED_SECURITY_PROFILES:
        errors.append(
            "Expected .codex/config.toml to use an approved security profile: "
            + ", ".join(f"{approval}/{sandbox}" for approval, sandbox in sorted(APPROVED_SECURITY_PROFILES))
            + "."
        )

    sandbox = config.get("sandbox_workspace_write", {})
    if config.get("sandbox_mode") == "workspace-write" and sandbox.get("network_access") is not False:
        errors.append("Expected sandbox_workspace_write.network_access = false.")

    agents_table = config.get("agents")
    if not isinstance(agents_table, dict):
        errors.append("Missing [agents] table in .codex/config.toml.")
        agents_table = {}

    for key in ("max_threads", "max_depth"):
        value = agents_table.get(key)
        if not isinstance(value, int) or value < 1:
            errors.append(f"Expected agents.{key} to be a positive integer.")

    registered_agent_names = set()
    for name in EXPECTED_AGENT_NAMES:
        agent_entry = agents_table.get(name)
        if not isinstance(agent_entry, dict):
            errors.append(f"Missing [agents.{name}] registration in .codex/config.toml.")
            continue
        config_file = agent_entry.get("config_file")
        if not isinstance(config_file, str) or not config_file:
            errors.append(f"Agent registration {name} is missing config_file.")
            continue

        registered_agent_names.add(name)
        agent_path = (CONFIG_PATH.parent / config_file).resolve()
        if not agent_path.exists():
            errors.append(f"Agent config file does not exist: {config_file}")
            continue

        agent_config = load_toml(agent_path)
        for required_key in ("name", "description", "developer_instructions"):
            value = agent_config.get(required_key)
            if not isinstance(value, str) or not value.strip():
                errors.append(f"{agent_path.relative_to(ROOT)} is missing {required_key}.")

        if agent_config.get("name") != name:
            errors.append(
                f"{agent_path.relative_to(ROOT)} declares name {agent_config.get('name')!r}, expected {name!r}."
            )

    agent_files = {path.stem for path in (ROOT / ".codex" / "agents").glob("*.toml")}
    if agent_files != EXPECTED_AGENT_NAMES:
        missing = EXPECTED_AGENT_NAMES - agent_files
        extra = agent_files - EXPECTED_AGENT_NAMES
        if missing:
            errors.append("Missing expected agent files: " + ", ".join(sorted(missing)))
        if extra:
            errors.append("Unexpected extra agent files: " + ", ".join(sorted(extra)))

    if registered_agent_names != EXPECTED_AGENT_NAMES:
        missing = EXPECTED_AGENT_NAMES - registered_agent_names
        if missing:
            errors.append("Missing registered agents: " + ", ".join(sorted(missing)))

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Codex configuration checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
