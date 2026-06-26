"""Validate the bounded AIDE Lite operational control plane."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
PILOT_PATH = ROOT / ".aide" / "pilot.toml"
BRIDGE_PROFILE_PATH = ROOT / ".aide" / "project_bridge_profile.toml"
AIDE_LITE_LOCK_PATH = ROOT / ".aide" / "aide_lite.lock.toml"
INTEGRATION_PLAN_PATH = ROOT / "docs" / "roadmap" / "aide-to-screensave-integration-plan.md"
CAPABILITY_BINDINGS = ROOT / "tools" / "project_adapter" / "capability_bindings.json"
ARTIFACT_PROFILE_AUDIT_ROOTS = ROOT / "tools" / "project_adapter" / "artifact_profile_audit_roots.json"
GITIGNORE_PATH = ROOT / ".gitignore"

REQUIRED_PATHS = [
    ROOT / ".aide" / "README.md",
    PILOT_PATH,
    AIDE_LITE_LOCK_PATH,
    BRIDGE_PROFILE_PATH,
    ROOT / ".aide" / "profile.yaml",
    INTEGRATION_PLAN_PATH,
    ROOT / ".aide" / "work_units" / "truth-proof-baseline.toml",
    ROOT / ".aide" / "evidence_packets" / "README.md",
    ROOT / ".aide" / "guidance" / "codex.md",
]


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_path(value: object, label: str, errors: list[str]) -> None:
    if not isinstance(value, str) or not value:
        errors.append(f"{label} must be a non-empty path string.")
        return
    path = (ROOT / value).resolve()
    require(path.exists(), f"{label} does not exist: {value}", errors)


def scan_runtime_dependency_references(errors: list[str]) -> None:
    for root_name in ("platform", "products"):
        root = ROOT / root_name
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix.lower() in {".png", ".jpg", ".jpeg", ".gif", ".ico", ".bmp"}:
                continue
            try:
                text = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            if ".aide" in text or "AIDE" in text:
                errors.append(f"Runtime/product tree must not reference AIDE during the pilot: {path.relative_to(ROOT)}")


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing AIDE pilot path: {path.relative_to(ROOT)}", errors)

    pilot = load_toml(PILOT_PATH)
    require(pilot.get("schema_version") == 1, ".aide/pilot.toml schema_version must be 1.", errors)
    require(pilot.get("status") == "aide-lite-operational", ".aide/pilot.toml status must remain aide-lite-operational.", errors)
    require(pilot.get("source_strategy") == "pinned-lite-safe-import", ".aide/pilot.toml source_strategy must be pinned-lite-safe-import.", errors)
    require(pilot.get("import_mode") == "safe", ".aide/pilot.toml import_mode must remain safe.", errors)
    require(pilot.get("runtime_mode") == "external-sidecar-only", ".aide/pilot.toml runtime_mode must remain external-sidecar-only.", errors)
    require(pilot.get("runtime_dependency_allowed") is False, "AIDE runtime dependency must be disabled.", errors)
    require(pilot.get("automatic_merging_allowed") is False, "AIDE automatic merging must be disabled.", errors)
    require(pilot.get("product_runtime_dependency_allowed") is False, "AIDE product runtime dependency must be disabled.", errors)
    require(pilot.get("source_mutation_allowed") is False, "AIDE source mutation must be disabled.", errors)

    for label, value in pilot.get("authority", {}).items():
        require_path(value, f"authority.{label}", errors)
    for label, value in pilot.get("validators", {}).items():
        require_path(value, f"validators.{label}", errors)
    for index, item in enumerate(pilot.get("work_units", [])):
        require_path(item.get("path"), f"work_units[{index}].path", errors)
    for index, item in enumerate(pilot.get("evidence_packets", [])):
        require_path(item.get("path"), f"evidence_packets[{index}].path", errors)

    aide_lite_lock = load_toml(AIDE_LITE_LOCK_PATH)
    require(aide_lite_lock.get("schema_version") == 1, ".aide/aide_lite.lock.toml schema_version must be 1.", errors)
    require(aide_lite_lock.get("status") == "aide-lite-operational", ".aide/aide_lite.lock.toml must remain aide-lite-operational.", errors)
    require(
        aide_lite_lock.get("source", {}).get("pinned_commit") == "492faa4f1a8280ba67954aa4fc252e79f2e19c15",
        ".aide/aide_lite.lock.toml must pin the reviewed AIDE Lite commit.",
        errors,
    )
    require(aide_lite_lock.get("import_mode") == "safe", ".aide/aide_lite.lock.toml import_mode must remain safe.", errors)
    require(
        aide_lite_lock.get("runtime_mode") == "external-sidecar-only",
        ".aide/aide_lite.lock.toml runtime_mode must remain external-sidecar-only.",
        errors,
    )
    require(
        aide_lite_lock.get("runtime_dependency_allowed") is False,
        ".aide/aide_lite.lock.toml must not allow runtime dependency.",
        errors,
    )
    require(aide_lite_lock.get("automatic_merge_allowed") is False, ".aide/aide_lite.lock.toml must not allow automatic merge.", errors)
    require(aide_lite_lock.get("source_mutation_allowed") is False, ".aide/aide_lite.lock.toml must not allow source mutation.", errors)
    require(
        aide_lite_lock.get("pack", {}).get("manifest_sha256") == "E02EFBB64036F0E73AF2856EE6066B1B6D4945C490EE0788FA1352A4DB371B4B",
        ".aide/aide_lite.lock.toml must record the regenerated pack manifest digest.",
        errors,
    )
    require(
        aide_lite_lock.get("distribution_manifest_v1", {}).get("status") == "unaccepted",
        ".aide/aide_lite.lock.toml must record DistributionManifest v1 as unaccepted.",
        errors,
    )

    bridge_profile = load_toml(BRIDGE_PROFILE_PATH)
    bindings = load_json(CAPABILITY_BINDINGS)
    audit_roots = load_json(ARTIFACT_PROFILE_AUDIT_ROOTS)
    require(bridge_profile.get("schema_version") == 1, ".aide/project_bridge_profile.toml schema_version must be 1.", errors)
    require(
        bridge_profile.get("status") == "aide-lite-operational",
        ".aide/project_bridge_profile.toml must remain aide-lite-operational.",
        errors,
    )
    for label, value in bridge_profile.get("authority", {}).items():
        if label == "output_root":
            require(
                value == "out/aide/screensave-project-adapter/invocations",
                "bridge_profile.authority.output_root must be the contained AIDE adapter output root.",
                errors,
            )
            continue
        require_path(value, f"bridge_profile.authority.{label}", errors)
    bridge = bridge_profile.get("bridge", {})
    require(bridge.get("mutation_allowed") is False, "ScreenSave bridge must not allow mutation.", errors)
    require(bridge.get("automatic_merge_allowed") is False, "ScreenSave bridge must not allow automatic merge.", errors)
    require(bridge.get("worker_runtime_required") is False, "ScreenSave bridge must not require AIDE worker runtime.", errors)
    readiness = bridge_profile.get("readiness", {})
    require(readiness.get("aide_lite_operational") == "ready-now", "AIDE Lite operational mode must remain ready-now.", errors)
    require(
        readiness.get("screen_save_fixed_capabilities") == "screensave-side-admitted",
        "ScreenSave fixed capabilities must be ScreenSave-side admitted.",
        errors,
    )
    require(
        readiness.get("contained_generated_proof_commands") == "screensave-side-admitted",
        "contained generated proof commands must be ScreenSave-side admitted.",
        errors,
    )
    require(
        readiness.get("aide_side_deterministic_bridge") == "screensave-local-bindings-ready",
        "AIDE-side deterministic bridge readiness must reflect ScreenSave-local bindings that are ready now.",
        errors,
    )
    require(
        bridge.get("aide_side_bridge_admission") == "canonical-aide-repo-acceptance-pending",
        "bridge.aide_side_bridge_admission must preserve the separate canonical AIDE acceptance boundary.",
        errors,
    )
    require(readiness.get("coding_agent_data_pack_proposal") == "later", "coding-agent data-pack proposal must remain later.", errors)
    require(
        readiness.get("source_patch_preview_apply") == "blocked",
        "source patch preview/apply must remain blocked.",
        errors,
    )
    require(
        readiness.get("automatic_product_promotion_or_release") == "deliberately-excluded",
        "automatic product promotion/release must remain deliberately-excluded.",
        errors,
    )
    require(
        readiness.get("external_aide_runtime_admission") == "outside-this-repository",
        "external AIDE runtime admission must stay outside this repository.",
        errors,
    )
    profile_names = {item.get("name") for item in bridge_profile.get("capabilities", [])}
    binding_names = {item.get("name") for item in bindings.get("capabilities", [])}
    require(profile_names == binding_names, "ScreenSave bridge profile capabilities must match capability bindings.", errors)
    forbidden_generic_capabilities = {"screensave.command", "screensave.run", "screensave.exec"}
    require(
        not (profile_names | binding_names) & forbidden_generic_capabilities,
        "ScreenSave bridge must not expose a generic command/run/exec capability.",
        errors,
    )
    capability_status = {item.get("name"): item.get("status") for item in bridge_profile.get("capabilities", [])}
    for name in {
        "screensave.project.status",
        "screensave.project.capabilities",
        "screensave.catalog.read",
        "screensave.profiles.read",
        "screensave.validation.t0",
        "screensave.validation.t1",
        "screensave.validation.t2",
        "screensave.validation.t3",
    }:
        require(
            capability_status.get(name) == "screensave-side-admitted-read-only",
            f"{name} must remain ScreenSave-side admitted as read-only.",
            errors,
        )
    for name in {
        "screensave.build.windows-current-x86",
        "screensave.build.windows-current-tools",
        "screensave.proof.nocturne.render",
        "screensave.proof.capture.compare",
        "screensave.artifact.pe.audit",
        "screensave.proof.nocturne.reference-v0.v1",
        "screensave.proof.nocturne.reference-v0.v2",
        "screensave.proof.ricochet.reference-v1.v1",
        "screensave.proof.ricochet.reference-v1.v2",
        "screensave.proof.plasma-v2.reference-preview.v2",
        "screensave.proof.portable-v2.equivalence",
        "screensave.bundle.portable-v2.equivalence",
        "screensave.bundle.nocturne.reference-v0.v1",
        "screensave.bundle.nocturne.reference-v0.v2",
        "screensave.bundle.ricochet.reference-v1.v1",
        "screensave.bundle.ricochet.reference-v1.v2",
        "screensave.bundle.plasma-v2.reference-preview.v2",
    }:
        require(
            capability_status.get(name) == "screensave-side-admitted-contained-output",
            f"{name} must remain ScreenSave-side admitted only as contained generated output.",
            errors,
        )
    require(
        bindings.get("output_root") == "out/aide/screensave-project-adapter/invocations",
        "ScreenSave bridge capability bindings must use the out/aide output root.",
        errors,
    )
    require(
        "windows_current_tool" in {item.get("key") for item in audit_roots.get("profiles", [])},
        "ScreenSave bridge audit roots must include the current tool artifact profile.",
        errors,
    )

    gitignore = GITIGNORE_PATH.read_text(encoding="utf-8")
    require(".aide.local/" in gitignore, ".gitignore must ignore .aide.local/.", errors)

    integration_plan = INTEGRATION_PLAN_PATH.read_text(encoding="utf-8") if INTEGRATION_PLAN_PATH.exists() else ""
    for needle in (
        "ScreenSave does not need to become an AIDE project.",
        "AIDE Lite operational safe import",
        "Deterministic fixed command",
        "Worker session",
        "ScreenSave-local fixed capability bindings are usable now",
        "DistributionManifest v1 remains relevant only to AIDE self-update",
        "screensave.command",
        "Automatic product promotion or release",
        "deliberately-excluded",
    ):
        require(needle in integration_plan, f"AIDE integration plan must preserve: {needle}", errors)

    scan_runtime_dependency_references(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE pilot checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
