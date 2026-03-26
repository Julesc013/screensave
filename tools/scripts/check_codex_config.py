"""Validate the project-scoped Codex configuration and registered custom agents."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONFIG_PATH = ROOT / ".codex" / "config.toml"

EXPECTED_AGENT_NAMES = {
    "repo_explorer",
    "repo_architect",
    "implementer",
    "reviewer",
}


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def main() -> int:
    config = load_toml(CONFIG_PATH)
    errors = []

    if config.get("model") != "gpt-5.4":
        errors.append("Expected .codex/config.toml to set model = 'gpt-5.4'.")
    if config.get("approval_policy") != "on-request":
        errors.append("Expected .codex/config.toml to set approval_policy = 'on-request'.")
    if config.get("sandbox_mode") != "workspace-write":
        errors.append("Expected .codex/config.toml to set sandbox_mode = 'workspace-write'.")

    sandbox = config.get("sandbox_workspace_write", {})
    if sandbox.get("network_access") is not False:
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
