"""Validate AIDE evidence for Plasma v2 instrument repair."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "evidence" / "plasma-v2-instrument-repair.toml"
INDEX = ROOT / ".aide" / "evidence" / "index.toml"
SUMMARY = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "aide-repair-summary.json"
VISUALINTENT_SUMMARY = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "visualintent" / "aide-repair-summary.json"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def main() -> int:
    errors: list[str] = []
    for path in (LEDGER, INDEX, SUMMARY, VISUALINTENT_SUMMARY):
        require(path.exists(), f"Missing AIDE instrument repair evidence path: {path.relative_to(ROOT)}", errors)
    if not errors:
        ledger = load_toml(LEDGER)
        summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
        visualintent_summary = json.loads(VISUALINTENT_SUMMARY.read_text(encoding="utf-8"))
        index_text = INDEX.read_text(encoding="utf-8")
        require(ledger.get("status") == "active", "AIDE instrument repair ledger must be active.", errors)
        require(ledger.get("network_calls") is False, "AIDE instrument repair ledger must be network-free.", errors)
        require(ledger.get("provider_or_model_calls") is False, "AIDE instrument repair ledger must be model-free.", errors)
        require(ledger.get("runtime_dependency_allowed") is False, "AIDE must not become a runtime dependency.", errors)
        for key in (
            "source_mutation_by_aide",
            "automatic_apply",
            "automatic_merge",
            "automatic_release",
            "stable_promotion",
            "compatibility_certification",
            "final_artistic_acceptance",
        ):
            require(ledger.get(key) is False, f"AIDE instrument repair ledger {key} must be false.", errors)
        claims = ledger.get("claims", {})
        refs = ledger.get("refs", {})
        visualintent = ledger.get("visualintent", {})
        visualintent_refs = ledger.get("visualintent_refs", {})
        require(claims.get("agentic_session") == "proposal-only", "AIDE agentic session must remain proposal-only.", errors)
        require(claims.get("work_unit") == "SS-PLV2-IR-REPAIR-001", "AIDE ledger must record the PAW-I-R2 repair WorkUnit.", errors)
        require(claims.get("repair_class") == "instrument_architecture_gap", "AIDE ledger must record the instrument architecture repair class.", errors)
        require(claims.get("blocker") == "legacy_preset_authority_removed", "AIDE ledger must record the repaired legacy authority blocker.", errors)
        require(claims.get("legacy_preset_authority_removed") is True, "AIDE ledger must record the legacy authority blocker as cleared.", errors)
        require(claims.get("legacy_authority_report") == "pass", "AIDE ledger must record the legacy authority report pass.", errors)
        require(claims.get("migration_report") == "pass", "AIDE ledger must record the migration report pass.", errors)
        require(claims.get("workbench_shell_validation") == "pass", "AIDE ledger must record the Workbench validation pass.", errors)
        require(claims.get("remaining_blocker") == "SS-PLV2-I-REPAIR-001", "AIDE summary must record the remaining stable artistic verdict blocker.", errors)
        require(
            "visualintent_candidates_reduce_to_plasma_spec" not in claims.get("remaining_blockers", []),
            "AIDE legacy ledger remaining blockers must no longer list the VisualIntent blocker after Turn 2.",
            errors,
        )
        for ref_key in [
            "legacy_authority_report",
            "migration_report",
            "workbench_inspection",
            "instrument_architecture_reaudit",
        ]:
            require(ref_key in refs, f"AIDE ledger missing repair evidence ref {ref_key}.", errors)
        require(summary.get("status") == "pass", "AIDE repair summary status must pass.", errors)
        require(summary.get("work_unit") == "SS-PLV2-IR-REPAIR-001", "AIDE repair summary must record SS-PLV2-IR-REPAIR-001.", errors)
        require(summary.get("repair_class") == "instrument_architecture_gap", "AIDE repair summary must record instrument_architecture_gap.", errors)
        require(summary.get("blocker") == "legacy_preset_authority_removed", "AIDE repair summary must record the repaired blocker.", errors)
        require(summary.get("legacy_preset_authority_removed") is True, "AIDE repair summary must record the legacy authority blocker as cleared.", errors)
        require(summary.get("agentic", {}).get("agent_mode") == "proposal-only", "AIDE repair summary must record proposal-only mode.", errors)
        require(summary.get("agentic", {}).get("task") == "SS-PLV2-IR-REPAIR-001", "AIDE repair summary must record the PAW-I-R2 agentic task.", errors)
        require(summary.get("agentic", {}).get("source_mutation_by_aide") is False, "AIDE repair summary must record no source mutation.", errors)
        require(summary.get("plans", {}).get("instrument_architecture_gap", {}).get("matching_count") == 1, "AIDE repair summary must record one instrument_architecture_gap repair.", errors)
        require(summary.get("plans", {}).get("release_readiness_gap", {}).get("matching_count") == 1, "AIDE repair summary must record one release_readiness_gap blocker.", errors)
        require(summary.get("plans", {}).get("proof_baseline_drift", {}).get("matching_count") == 0, "AIDE repair summary must record no proof baseline drift repairs.", errors)
        require(summary.get("plans", {}).get("workbench_surface_drift", {}).get("matching_count") == 0, "AIDE repair summary must record no Workbench surface drift repairs.", errors)
        repair_evidence = set(summary.get("repair_evidence", []))
        for ref in [
            "validation/captures/plasma-v2/instrument-audit/legacy-authority-report.json",
            "validation/captures/plasma-v2/instrument-audit/migration-report.json",
            "validation/captures/plasma-v2/instrument-audit/workbench-inspection.json",
            "validation/captures/plasma-v2/stable-promotion/instrument-architecture-audit.json",
        ]:
            require(ref in repair_evidence, f"AIDE repair summary missing repair evidence {ref}.", errors)
        remaining_ids = {item.get("id") for item in summary.get("remaining_blockers", []) if isinstance(item, dict)}
        require(
            {"visualintent_candidates_reduce_to_plasma_spec", "SS-PLV2-I-REPAIR-001"} <= remaining_ids,
            "AIDE repair summary must preserve VisualIntent and final artistic acceptance blockers.",
            errors,
        )
        require("plasma-v2-instrument-repair" in index_text, "AIDE evidence index must track the instrument repair ledger.", errors)
        require(
            visualintent.get("work_unit") == "SS-PLV2-IR-REPAIR-002",
            "AIDE ledger must record the VisualIntent repair WorkUnit.",
            errors,
        )
        require(
            visualintent.get("repair_class") == "instrument_architecture_gap",
            "AIDE ledger must record the VisualIntent repair class.",
            errors,
        )
        require(
            visualintent.get("blocker") == "visualintent_candidates_reduce_to_plasma_spec",
            "AIDE ledger must record the VisualIntent blocker.",
            errors,
        )
        require(
            visualintent.get("visualintent_candidates_reduce_to_plasma_spec") is True,
            "AIDE ledger must record the VisualIntent blocker as cleared.",
            errors,
        )
        require(visualintent.get("spec_reduction_report") == "pass", "AIDE ledger must record the spec reduction report pass.", errors)
        require(visualintent.get("proof_summary") == "pass", "AIDE ledger must record the proof summary pass.", errors)
        require(visualintent.get("workbench_shell_validation") == "pass", "AIDE ledger must record Workbench validation pass for VisualIntent.", errors)
        require(visualintent.get("agentic_session") == "proposal-only", "AIDE VisualIntent agentic session must remain proposal-only.", errors)
        require(visualintent.get("source_mutation_by_aide") is False, "AIDE VisualIntent ledger must record no source mutation.", errors)
        require(visualintent.get("remaining_blockers") == ["final stable artistic acceptance"], "AIDE VisualIntent ledger must leave only final artistic acceptance.", errors)
        for ref_key in [
            "spec_reduction_report",
            "proof_summary",
            "workbench_inspection",
            "instrument_architecture_reaudit",
        ]:
            require(ref_key in visualintent_refs, f"AIDE VisualIntent ledger missing repair evidence ref {ref_key}.", errors)
        require(visualintent_summary.get("status") == "pass", "AIDE VisualIntent repair summary status must pass.", errors)
        require(
            visualintent_summary.get("work_unit") == "SS-PLV2-IR-REPAIR-002",
            "AIDE VisualIntent repair summary must record SS-PLV2-IR-REPAIR-002.",
            errors,
        )
        require(
            visualintent_summary.get("blocker") == "visualintent_candidates_reduce_to_plasma_spec",
            "AIDE VisualIntent repair summary must record the repaired blocker.",
            errors,
        )
        require(
            visualintent_summary.get("visualintent_candidates_reduce_to_plasma_spec") is True,
            "AIDE VisualIntent repair summary must record the blocker as cleared.",
            errors,
        )
        require(
            visualintent_summary.get("agentic", {}).get("agent_mode") == "proposal-only",
            "AIDE VisualIntent repair summary must record proposal-only agentic mode.",
            errors,
        )
        require(
            visualintent_summary.get("agentic", {}).get("source_mutation_by_aide") is False,
            "AIDE VisualIntent repair summary must record no source mutation.",
            errors,
        )
        visualintent_evidence = set(visualintent_summary.get("repair_evidence", []))
        for ref in [
            "validation/captures/plasma-v2/instrument-audit/visualintent/spec-reduction-report.json",
            "validation/captures/plasma-v2/instrument-audit/visualintent/proof-summary.json",
            "validation/captures/plasma-v2/instrument-audit/workbench-inspection.json",
            "validation/captures/plasma-v2/stable-promotion/instrument-architecture-audit.json",
        ]:
            require(ref in visualintent_evidence, f"AIDE VisualIntent repair summary missing repair evidence {ref}.", errors)
        visualintent_remaining_ids = {item.get("id") for item in visualintent_summary.get("remaining_blockers", []) if isinstance(item, dict)}
        require(
            visualintent_remaining_ids == {"SS-PLV2-I-REPAIR-001"},
            "AIDE VisualIntent repair summary must leave only the final artistic acceptance repair.",
            errors,
        )
        require("plasma-v2-visualintent-spec-reduction" in index_text, "AIDE evidence index must track VisualIntent spec reduction.", errors)
        require("plasma-v2-visualintent-proof-summary" in index_text, "AIDE evidence index must track VisualIntent proof summary.", errors)
        require("human artistic acceptance remain authoritative" in index_text, "AIDE evidence index must preserve the human acceptance boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 AIDE repair evidence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
