"""Validate ScreenSave-local AIDE PAW-CX operational repair artifacts."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
TOKEN_BASELINES = ROOT / ".aide" / "reports" / "token-baselines.yaml"
FAILURE_TAXONOMY = ROOT / ".aide" / "controller" / "failure-taxonomy.yaml"
CONTROLLER_README = ROOT / ".aide" / "controller" / "README.md"
TEST_MANIFEST = ROOT / ".aide" / "tests" / "test_manifest.yaml"
TEST_IMPACT_MAP = ROOT / ".aide" / "tests" / "test_impact_map.yaml"
AIDE_OPS = ROOT / "tools" / "aideops" / "screensave_aide.py"
AIDE_OPS_README = ROOT / "tools" / "aideops" / "README.md"
PAW_C1_WORK_UNIT = ROOT / ".aide" / "work_units" / "paw-c1-portable-v2-runtime-equivalence.toml"
PAW_C1_ROADMAP = ROOT / "docs" / "roadmap" / "paw-c1-portable-v2-runtime-equivalence.md"

EXPECTED_BASELINES = {
    "root_history_baseline",
    "repo_context_baseline",
    "review_baseline",
    "token_survival_baseline",
}

EXPECTED_FAILURE_CLASSES = {
    "context_missing",
    "packet_too_large",
    "token_budget_exceeded",
    "token_regression",
    "adapter_drift",
    "verifier_gap",
    "verifier_fail",
    "review_packet_incomplete",
    "golden_task_fail",
    "unclear_acceptance",
    "validation_failure",
    "policy_violation",
    "stale_artifact",
    "missing_evidence",
    "secret_risk",
    "unknown",
}

EXPECTED_SCREEN_SAVE_GOLDEN_TASKS = {
    "screensave-aide-remains-optional",
    "screensave-no-generic-exec-capability",
    "screensave-nocturne-baseline-exact",
    "screensave-ricochet-baselines-exact",
    "screensave-workbench-uses-libsslab",
    "screensave-proof-does-not-imply-artistic-acceptance",
    "screensave-pe-audit-does-not-imply-os-certification",
    "screensave-v2-headers-have-no-native-types",
    "screensave-v2-structs-are-size-version-prefixed",
    "screensave-v1-scr-build-remains-operational",
}


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def yaml_ids(text: str) -> set[str]:
    ids: set[str] = set()
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("- id:"):
            ids.add(stripped.split(":", 1)[1].strip())
    return ids


def yaml_names(text: str) -> set[str]:
    names: set[str] = set()
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("- name:"):
            names.add(stripped.split(":", 1)[1].strip())
    return names


def validate_static_files(errors: list[str]) -> None:
    for path in [
        TOKEN_BASELINES,
        FAILURE_TAXONOMY,
        CONTROLLER_README,
        TEST_MANIFEST,
        TEST_IMPACT_MAP,
        AIDE_OPS,
        AIDE_OPS_README,
        PAW_C1_WORK_UNIT,
        PAW_C1_ROADMAP,
    ]:
        require(path.exists(), f"Missing AIDE operational path: {path.relative_to(ROOT)}", errors)

    if TOKEN_BASELINES.exists():
        text = read_text(TOKEN_BASELINES)
        require("schema_version: aide.token-baselines.v0" in text, "token baselines must declare schema.", errors)
        require(yaml_names(text) == EXPECTED_BASELINES, "token baselines must define the four ScreenSave baselines.", errors)
        for needle in [
            "contracts/screensave_doctrine_v1.md",
            "contracts/portable_semantics_v2.md",
            ".aide/context/latest-task-packet.md",
        ]:
            require(needle in text, f"token baselines missing path: {needle}", errors)

    if FAILURE_TAXONOMY.exists():
        text = read_text(FAILURE_TAXONOMY)
        require("schema_version: aide.failure-taxonomy.v0" in text, "failure taxonomy must declare schema.", errors)
        require(yaml_ids(text) == EXPECTED_FAILURE_CLASSES, "failure taxonomy must match AIDE Lite controller failure classes.", errors)
        require("492faa4f1a8280ba67954aa4fc252e79f2e19c15" in text, "failure taxonomy must record the imported AIDE Lite source pin.", errors)

    if TEST_IMPACT_MAP.exists():
        text = read_text(TEST_IMPACT_MAP)
        for tier in ["T0:", "T1:", "T2:", "T3:"]:
            require(tier in text, f"test impact map missing {tier}", errors)
        require(yaml_ids(text) >= EXPECTED_SCREEN_SAVE_GOLDEN_TASKS, "ScreenSave golden task definitions are incomplete.", errors)
        for forbidden in ["LLM judge", "screensave.command", "screensave.run", "screensave.exec"]:
            if forbidden == "LLM judge":
                require(forbidden not in text, "ScreenSave golden tasks must not use an LLM judge.", errors)
            else:
                require(forbidden in text, f"ScreenSave golden tasks must explicitly forbid {forbidden}.", errors)


def validate_operator(errors: list[str]) -> None:
    if not AIDE_OPS.exists():
        return
    text = read_text(AIDE_OPS)
    for needle in [
        "AIDE_LITE",
        "OPERATION_ROOT",
        "command_plan",
        "bootstrap",
        "preflight",
        "postflight",
        "full-check",
        "provider_calls",
        "network_calls",
        "source_mutation_outside_admitted_paths",
        "validation_tier_selection",
        "changed_file_summary_before",
        "changed_file_summary_after",
        "receipt_kind",
        "blocked_step_list",
    ]:
        require(needle in text, f"screensave_aide.py missing expected operator anchor: {needle}", errors)
    for forbidden in ["requests", "openai", "urllib.request", "git push", "git merge"]:
        require(forbidden not in text, f"screensave_aide.py must not use {forbidden}.", errors)

    operation_id = "check-aide-operational-status"
    result = subprocess.run(
        [sys.executable, str(AIDE_OPS), "--operation-id", operation_id, "status"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(result.returncode == 0, f"screensave_aide.py status failed: {result.stderr}", errors)
    receipt_path = ROOT / "out" / "aide" / "operations" / operation_id / "operation-receipt.json"
    require(receipt_path.exists(), "screensave_aide.py status must write operation-receipt.json.", errors)
    if receipt_path.exists():
        receipt = json.loads(read_text(receipt_path))
        require(receipt.get("schema_version") == "screensave.aide-operation-receipt.v0", "operation receipt schema mismatch.", errors)
        require(receipt.get("provider_calls") is False, "operation receipt must record provider_calls false.", errors)
        require(receipt.get("model_calls") is False, "operation receipt must record model_calls false.", errors)
        require(receipt.get("network_calls") is False, "operation receipt must record network_calls false.", errors)
        require(
            receipt.get("source_mutation_outside_admitted_paths") is False,
            "operation receipt must record source_mutation_outside_admitted_paths false.",
            errors,
        )
        require(
            receipt.get("aide", {}).get("aide_source_pin") == "492faa4f1a8280ba67954aa4fc252e79f2e19c15",
            "operation receipt must include the pinned AIDE source revision.",
            errors,
        )

    if PAW_C1_WORK_UNIT.exists():
        text = read_text(PAW_C1_WORK_UNIT)
        for task_id in ["SS-PV2-03", "SS-PV2-04", "SS-PV2-05", "SS-PV2-06", "SS-PV2-07", "SS-PV2-08"]:
            require(task_id in text, f"PAW-C1 work-unit packet missing {task_id}.", errors)
        require("d57669d737a2e561f0f86f699be9b280347d683e" in text, "PAW-C1 work-unit packet must pin the d57669d launch point.", errors)
        require("preflight_required = true" in text, "PAW-C1 work-unit packet must require preflight.", errors)
        require("postflight_required = true" in text, "PAW-C1 work-unit packet must require postflight.", errors)

    if PAW_C1_ROADMAP.exists():
        text = read_text(PAW_C1_ROADMAP)
        for needle in [
            "Make portable v2 executable through the two proof canaries",
            "Do not implement Plasma v2 runtime",
            "Portable meaning.",
            "Native delivery.",
            "Deterministic proof.",
            "Optional automation.",
        ]:
            require(needle in text, f"PAW-C1 roadmap missing expected boundary text: {needle}", errors)


def main() -> int:
    errors: list[str] = []
    validate_static_files(errors)
    validate_operator(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE operational checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
