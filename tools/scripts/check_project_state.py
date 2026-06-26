"""Validate and summarize the canonical ScreenSave project-state authority."""

from __future__ import annotations

import argparse
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE_PATH = ROOT / "PROJECT_STATE.toml"
VERSION_PATH = ROOT / "VERSION.toml"
README_PATH = ROOT / "README.md"
VERSION_HEADER = ROOT / "platform" / "include" / "screensave" / "version.h"

REQUIRED_TOP_LEVEL = {
    "schema_version",
    "state_id",
    "as_of",
    "authority",
    "release",
    "development",
    "compatibility",
    "proof_kernel",
    "proof_kernel_v1",
    "portable_v2",
    "queues",
    "doctrine",
    "catalog",
    "project_adapter",
    "aide_lite",
    "plasma",
    "build_profiles",
    "validators",
}

REQUIRED_QUEUE_STATUS = "historical-superseded"


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_path(value: object, label: str, errors: list[str]) -> None:
    if not isinstance(value, str) or not value:
        errors.append(f"{label} must be a non-empty path string.")
        return
    path = (ROOT / value).resolve()
    require(path.exists(), f"{label} does not exist: {value}", errors)


def validate_state(state: dict) -> list[str]:
    errors: list[str] = []

    missing = REQUIRED_TOP_LEVEL - set(state)
    for key in sorted(missing):
        errors.append(f"PROJECT_STATE.toml is missing top-level key {key!r}.")

    authority = state.get("authority", {})
    release = state.get("release", {})
    development = state.get("development", {})
    compatibility = state.get("compatibility", {})
    proof_kernel = state.get("proof_kernel", {})
    proof_kernel_v1 = state.get("proof_kernel_v1", {})
    portable_v2 = state.get("portable_v2", {})
    plasma_v2 = state.get("plasma_v2", {})
    queues = state.get("queues", {})
    doctrine = state.get("doctrine", {})
    catalog_state = state.get("catalog", {})
    project_adapter = state.get("project_adapter", {})
    aide_lite = state.get("aide_lite", {})
    validators = state.get("validators", {})
    plasma = state.get("plasma", {})
    version = load_toml(VERSION_PATH)
    catalog = load_toml(ROOT / "catalog" / "products.toml")

    require(authority.get("current_truth") == "truth-proof-baseline", "authority.current_truth must be truth-proof-baseline.", errors)
    require(authority.get("public_release") == release.get("tag"), "authority.public_release must match release.tag.", errors)
    require(queues.get("status") == REQUIRED_QUEUE_STATUS, "queues.status must be historical-superseded.", errors)
    require(development.get("ship_posture") == "GO_WITH_CAVEATS", "development.ship_posture must preserve GO_WITH_CAVEATS.", errors)
    require(compatibility.get("policy") == "evidence-classed", "compatibility.policy must be evidence-classed.", errors)
    require(authority.get("version_manifest") == "VERSION.toml", "authority.version_manifest must point to VERSION.toml.", errors)
    portable_status = portable_v2.get("status")
    if portable_status == "accepted":
        expected_active_program = (
            "plasma-v2-release-readiness"
            if plasma_v2.get("status") == "stable-candidate"
            else "plasma-v2-reference-slice"
        )
    else:
        expected_active_program = "portable-v2-seam"
    require(
        authority.get("active_program") == expected_active_program,
        f"authority.active_program must be {expected_active_program}.",
        errors,
    )
    require(
        development.get("active_program") == expected_active_program,
        f"development.active_program must be {expected_active_program}.",
        errors,
    )
    require(proof_kernel.get("status") == "complete", "proof_kernel.status must be complete.", errors)
    require(proof_kernel.get("milestone") == "proof-kernel-v0", "proof_kernel.milestone must remain proof-kernel-v0.", errors)
    require(proof_kernel_v1.get("status") == "complete", "proof_kernel_v1.status must be complete.", errors)
    require(
        proof_kernel_v1.get("milestone") == "proof-kernel-v1-ricochet",
        "proof_kernel_v1.milestone must be proof-kernel-v1-ricochet.",
        errors,
    )
    require(proof_kernel_v1.get("completed_on") == "2026-06-26", "proof_kernel_v1.completed_on must be 2026-06-26.", errors)
    require(proof_kernel_v1.get("opened_next") == "portable-v2-seam", "proof_kernel_v1.opened_next must be portable-v2-seam.", errors)
    require(list(proof_kernel_v1.get("remaining", [])) == [], "proof_kernel_v1.remaining must be empty after Gate B closeout.", errors)
    require(
        "ricochet" in set(proof_kernel_v1.get("canary_products", [])),
        "proof_kernel_v1.canary_products must include ricochet.",
        errors,
    )
    require(portable_status in {"active", "accepted"}, "portable_v2.status must be active or accepted.", errors)
    require(portable_v2.get("milestone") == "portable-v2-seam", "portable_v2.milestone must be portable-v2-seam.", errors)
    require(portable_v2.get("gate") == "PAW-C", "portable_v2.gate must be PAW-C.", errors)
    require(
        {"nocturne", "ricochet"} <= set(portable_v2.get("porting_products", [])),
        "portable_v2.porting_products must include nocturne and ricochet.",
        errors,
    )
    if portable_status == "accepted":
        require(portable_v2.get("completed_on") == "2026-06-26", "portable_v2.completed_on must be 2026-06-26.", errors)
        require(portable_v2.get("accepted_by") == "check_gate_c_acceptance.py", "portable_v2.accepted_by must name the Gate C checker.", errors)
        require(portable_v2.get("opened_next") == "plasma-v2-reference-slice", "portable_v2.opened_next must be plasma-v2-reference-slice.", errors)
        require(list(portable_v2.get("remaining", [])) == [], "portable_v2.remaining must be empty after Gate C acceptance.", errors)

    if plasma_v2:
        require(plasma_v2.get("stable") is False, "plasma_v2.stable must remain false before release readiness.", errors)
        require(plasma_v2.get("release_promotion") == "blocked", "plasma_v2.release_promotion must remain blocked.", errors)
        require(
            plasma_v2.get("status") in {"reference-preview", "reviewed-preview", "stable-candidate"},
            "plasma_v2.status must be a recognized pre-stable status.",
            errors,
        )
        if plasma_v2.get("status") == "stable-candidate":
            require(plasma_v2.get("packc") == "v1-candidate", "plasma_v2.packc must be v1-candidate.", errors)
            require(
                plasma_v2.get("visual_review") == "stable-candidate-round-recorded",
                "plasma_v2.visual_review must record the stable-candidate round.",
                errors,
            )
            require(
                plasma_v2.get("artistic_acceptance") == "stable-candidate-review",
                "plasma_v2.artistic_acceptance must remain stable-candidate-review.",
                errors,
            )
            require(
                plasma_v2.get("opened_next") == "plasma-v2-release-readiness",
                "plasma_v2.opened_next must be plasma-v2-release-readiness.",
                errors,
            )
            for label in (
                "acceleration_matrix",
                "performance_envelope",
                "stable_candidate_review",
                "stable_candidate_gate",
            ):
                require_path(plasma_v2.get(label), f"plasma_v2.{label}", errors)
            require(
                "not stable release" in str(plasma_v2.get("claim_boundary", "")).lower(),
                "plasma_v2.claim_boundary must block stable release.",
                errors,
            )

    for label, value in (
        ("authority.product_catalog", authority.get("product_catalog")),
        ("authority.artifact_profiles", authority.get("artifact_profiles")),
        ("authority.artifact_sets", authority.get("artifact_sets")),
        ("authority.proof_profiles", authority.get("proof_profiles")),
        ("authority.product_doctrine", authority.get("product_doctrine")),
        ("authority.product_architecture", authority.get("product_architecture")),
        ("authority.generated_catalog_inventory", authority.get("generated_catalog_inventory")),
        ("authority.generated_proof_registry", authority.get("generated_proof_registry")),
        ("authority.version_manifest", authority.get("version_manifest")),
        ("authority.project_adapter", authority.get("project_adapter")),
        ("authority.build_controller", authority.get("build_controller")),
        ("authority.aide_pilot", authority.get("aide_pilot")),
        ("authority.aide_lite_lock", authority.get("aide_lite_lock")),
        ("authority.aide_profile", authority.get("aide_profile")),
        ("authority.aide_integration_plan", authority.get("aide_integration_plan")),
        ("authority.visual_intent_contract", authority.get("visual_intent_contract")),
        ("release.artifact_manifest", release.get("artifact_manifest")),
        ("release.checksums", release.get("checksums")),
        ("release.notes", release.get("notes")),
        ("release.known_limits", release.get("known_limits")),
        ("development.current_product_truth", development.get("current_product_truth")),
        ("development.ship_posture_source", development.get("ship_posture_source")),
        ("development.proof_record", development.get("proof_record")),
        ("development.proof_captures", development.get("proof_captures")),
        ("compatibility.binary_audit", compatibility.get("binary_audit")),
        ("proof_kernel.contract", proof_kernel.get("contract")),
        ("proof_kernel.surface_contract", proof_kernel.get("surface_contract")),
        ("proof_kernel.canary_evidence", proof_kernel.get("canary_evidence")),
        ("proof_kernel_v1.libsslab_abi", proof_kernel_v1.get("libsslab_abi")),
        ("proof_kernel_v1.libsslab_header", proof_kernel_v1.get("libsslab_header")),
        ("portable_v2.contract", portable_v2.get("contract")),
        ("plasma.stable.source", plasma.get("stable", {}).get("source")),
        ("plasma.experimental.source", plasma.get("experimental", {}).get("source")),
    ):
        require_path(value, label, errors)

    wave_files = queues.get("wave_files")
    if not isinstance(wave_files, list) or not wave_files:
        errors.append("queues.wave_files must be a non-empty list.")
    else:
        for index, value in enumerate(wave_files):
            require_path(value, f"queues.wave_files[{index}]", errors)
            if isinstance(value, str):
                wave = load_toml(ROOT / value)
                require(wave.get("status") == REQUIRED_QUEUE_STATUS, f"{value} must have status {REQUIRED_QUEUE_STATUS!r}.", errors)

    require(doctrine.get("schema_version") == 1, "doctrine.schema_version must be 1.", errors)
    require(
        doctrine.get("short_form") == "Portable meaning. Native delivery. Deterministic proof. Optional automation.",
        "doctrine.short_form must preserve the ScreenSave short doctrine.",
        errors,
    )
    require(doctrine.get("platform_rule") == "Share mechanics. Preserve meaning.", "doctrine.platform_rule must preserve the platform rule.", errors)
    require(
        "many product-owned semantic cores" in str(doctrine.get("semantic_rule", "")),
        "doctrine.semantic_rule must preserve product-owned semantic cores.",
        errors,
    )
    require(
        doctrine.get("aide_boundary") == "AIDE governs bounded development activity around ScreenSave. ScreenSave governs the product.",
        "doctrine.aide_boundary must preserve the AIDE product boundary.",
        errors,
    )
    require(
        "descriptive authoring intent only" in str(doctrine.get("visual_intent_rule", "")),
        "doctrine.visual_intent_rule must keep VisualIntent descriptive only.",
        errors,
    )
    require_path(doctrine.get("contract"), "doctrine.contract", errors)

    require(catalog_state.get("schema_version") == 1, "catalog.schema_version must be 1.", errors)
    for label, value in (
        ("catalog.source", catalog_state.get("source")),
        ("catalog.artifact_profiles", catalog_state.get("artifact_profiles")),
        ("catalog.artifact_sets", catalog_state.get("artifact_sets")),
        ("catalog.proof_profiles", catalog_state.get("proof_profiles")),
        ("catalog.generator", catalog_state.get("generator")),
        ("catalog.generated_inventory", catalog_state.get("generated_inventory")),
        ("catalog.generated_sources", catalog_state.get("generated_sources")),
        ("catalog.generated_table", catalog_state.get("generated_table")),
        ("catalog.generated_proof_registry", catalog_state.get("generated_proof_registry")),
        ("catalog.generated_proof_registry_header", catalog_state.get("generated_proof_registry_header")),
        ("catalog.generated_proof_registry_source", catalog_state.get("generated_proof_registry_source")),
    ):
        require_path(value, label, errors)

    require(project_adapter.get("schema_version") == 1, "project_adapter.schema_version must be 1.", errors)
    require(project_adapter.get("status") == "active", "project_adapter.status must be active.", errors)
    require(
        {"status", "capabilities", "catalog", "validate", "build", "render", "compare", "audit", "proof"} <= set(project_adapter.get("commands", [])),
        "project_adapter.commands must include status, capabilities, catalog, validate, build, render, compare, audit, and proof.",
        errors,
    )
    for label, value in (
        ("project_adapter.contract", project_adapter.get("contract")),
        ("project_adapter.command", project_adapter.get("command")),
        ("project_adapter.capability_bindings", project_adapter.get("capability_bindings")),
        ("project_adapter.receipt_schemas", project_adapter.get("receipt_schemas")),
        ("project_adapter.artifact_profile_audit_roots", project_adapter.get("artifact_profile_audit_roots")),
    ):
        require_path(value, label, errors)
    require(
        project_adapter.get("output_root") == "out/aide/screensave-project-adapter/invocations",
        "project_adapter.output_root must be the contained AIDE invocation output root.",
        errors,
    )
    require(
        "out/aide" in str(project_adapter.get("policy", ""))
        and "artifact-profile audit roots" in str(project_adapter.get("policy", ""))
        and "fixed build profiles" in str(project_adapter.get("policy", "")),
        "project_adapter.policy must mention out/aide containment, fixed build profiles, and artifact-profile audit roots.",
        errors,
    )

    require(aide_lite.get("status") == "aide-lite-operational", "aide_lite.status must be aide-lite-operational.", errors)
    require(
        aide_lite.get("admitted_commit") == "492faa4f1a8280ba67954aa4fc252e79f2e19c15",
        "aide_lite.admitted_commit must match the admitted AIDE Lite pin.",
        errors,
    )
    require(aide_lite.get("import_mode") == "safe", "aide_lite.import_mode must remain safe.", errors)
    require(
        aide_lite.get("runtime_mode") == "external-sidecar-only",
        "aide_lite.runtime_mode must remain external-sidecar-only.",
        errors,
    )
    require(aide_lite.get("source_mutation_allowed") is False, "aide_lite.source_mutation_allowed must remain false.", errors)
    require(aide_lite.get("automatic_merge_allowed") is False, "aide_lite.automatic_merge_allowed must remain false.", errors)
    require(aide_lite.get("general_worker_admitted") is False, "aide_lite.general_worker_admitted must remain false.", errors)
    require(
        aide_lite.get("distribution_manifest_v1_status") in {"unaccepted", "accepted-upstream"},
        "aide_lite.distribution_manifest_v1_status must be unaccepted or accepted-upstream.",
        errors,
    )
    if aide_lite.get("distribution_manifest_v1_status") == "accepted-upstream":
        require(
            aide_lite.get("project_lock_v0_status") == "accepted-upstream",
            "aide_lite.project_lock_v0_status must record accepted-upstream.",
            errors,
        )
        require(
            aide_lite.get("ownership_ledger_v1_status") == "accepted-upstream",
            "aide_lite.ownership_ledger_v1_status must record accepted-upstream.",
            errors,
        )
        require(
            aide_lite.get("local_process_host_fixture_status") == "accepted-upstream-fixture-only",
            "aide_lite.local_process_host_fixture_status must stay fixture-only.",
            errors,
        )
        require(aide_lite.get("preview_apply_rollback_admitted") is False, "preview/apply/rollback must remain blocked.", errors)
        require(aide_lite.get("automatic_release_allowed") is False, "automatic release must remain blocked.", errors)
    require(
        aide_lite.get("pack_manifest_sha256") == "E02EFBB64036F0E73AF2856EE6066B1B6D4945C490EE0788FA1352A4DB371B4B",
        "aide_lite.pack_manifest_sha256 must match the regenerated pack manifest digest.",
        errors,
    )

    for name, value in validators.items():
        require_path(value, f"validators.{name}", errors)

    validate_version_manifest(state, version, catalog, errors)

    readme = README_PATH.read_text(encoding="utf-8")
    for phrase in (
        state.get("state_id", ""),
        "Truth And Proof Baseline",
        "VERSION.toml",
        "GO WITH CAVEATS",
        "evidence-classed",
    ):
        require(isinstance(phrase, str) and phrase in readme, f"README.md must mention {phrase!r}.", errors)

    return errors


def validate_version_manifest(state: dict, version: dict, catalog: dict, errors: list[str]) -> None:
    release = state["release"]
    authority = state["authority"]
    compatibility = state["compatibility"]
    version_release = version.get("release", {})
    version_development = version.get("development", {})
    version_schemas = version.get("schemas", {})
    version_proof = version.get("proof", {})

    require(version.get("schema_version") == 1, "VERSION.toml schema_version must be 1.", errors)
    require(version_release.get("public_artifact") == release.get("tag"), "VERSION.toml release.public_artifact must match PROJECT_STATE release.tag.", errors)
    require(version_release.get("runtime_version") == "0.15.0", "VERSION.toml release.runtime_version must remain 0.15.0 for C16.", errors)
    require(version_release.get("status") == release.get("status"), "VERSION.toml release.status must match PROJECT_STATE release.status.", errors)
    require(version_development.get("current_state") == state.get("state_id"), "VERSION.toml development.current_state must match PROJECT_STATE state_id.", errors)
    require(version_development.get("development_head") == authority.get("development_head"), "VERSION.toml development.development_head must match PROJECT_STATE authority.", errors)
    require(
        version_development.get("active_milestone") == authority.get("active_program"),
        "VERSION.toml development.active_milestone must match PROJECT_STATE authority.active_program.",
        errors,
    )
    require(version_schemas.get("product_catalog") == catalog.get("schema_version"), "VERSION.toml schemas.product_catalog must match catalog schema_version.", errors)
    require(version_schemas.get("screensave_doctrine") == 1, "VERSION.toml schemas.screensave_doctrine must be 1.", errors)
    require(version_schemas.get("product_architecture") == 1, "VERSION.toml schemas.product_architecture must be 1.", errors)
    require(version_schemas.get("project_adapter") == 1, "VERSION.toml schemas.project_adapter must be 1.", errors)
    require(version_schemas.get("visual_intent") == 1, "VERSION.toml schemas.visual_intent must be 1.", errors)
    require(version_schemas.get("artifact_sets") == 1, "VERSION.toml schemas.artifact_sets must be 1.", errors)
    require(version_schemas.get("proof_profiles") == 1, "VERSION.toml schemas.proof_profiles must be 1.", errors)
    require(version_schemas.get("proof_bundle_normalized") == 1, "VERSION.toml schemas.proof_bundle_normalized must be 1.", errors)
    require(version.get("abi", {}).get("portable_contract") == "accepted-v2", "VERSION.toml abi.portable_contract must be accepted-v2 after Gate C.", errors)
    require(version_schemas.get("libsslab_abi") == 0, "VERSION.toml schemas.libsslab_abi must remain 0 for the ABI v0 contract record.", errors)
    require_path(version.get("contracts", {}).get("product_architecture"), "VERSION.toml contracts.product_architecture", errors)
    require_path(version.get("contracts", {}).get("visual_intent"), "VERSION.toml contracts.visual_intent", errors)
    require_path(version.get("contracts", {}).get("proof_bundle_v1"), "VERSION.toml contracts.proof_bundle_v1", errors)
    require_path(version.get("contracts", {}).get("libsslab_abi"), "VERSION.toml contracts.libsslab_abi", errors)
    require(version_proof.get("policy") == compatibility.get("policy"), "VERSION.toml proof.policy must match PROJECT_STATE compatibility.policy.", errors)
    require(version_proof.get("bundle_schema") == "proof-bundle-v0", "VERSION.toml proof.bundle_schema must preserve raw proof-bundle-v0.", errors)
    require(version_proof.get("normalized_bundle_schema") == "proof-bundle-v1", "VERSION.toml proof.normalized_bundle_schema must be proof-bundle-v1.", errors)
    require_path(version_proof.get("binary_audit_tool"), "VERSION.toml proof.binary_audit_tool", errors)
    require_path(version_proof.get("state_validator"), "VERSION.toml proof.state_validator", errors)

    header = VERSION_HEADER.read_text(encoding="utf-8")
    expected_runtime = version_release.get("runtime_version")
    expected_series = version_release.get("series")
    require(f'SCREENSAVE_VERSION_TEXT "{expected_runtime}"' in header, "version.h must match VERSION.toml release.runtime_version.", errors)
    require(f'SCREENSAVE_VERSION_SERIES "{expected_series}"' in header, "version.h must match VERSION.toml release.series.", errors)


def print_summary(state: dict) -> None:
    authority = state["authority"]
    release = state["release"]
    development = state["development"]
    compatibility = state["compatibility"]
    queues = state["queues"]
    portable_v2 = state["portable_v2"]
    plasma = state["plasma"]["stable"]

    print(f"State: {state['state_id']} ({state['as_of']})")
    print(f"Released public artifact: {release['tag']} [{release['status']}]")
    print(f"Development head: {authority['development_head']}")
    print(f"Active program: {authority['active_program']}")
    print(f"Release candidate: {authority['release_candidate']}")
    print(f"Queue authority: {queues['status']} ({queues['superseded_by']})")
    print(f"Compatibility policy: {compatibility['policy']} / default OS status: {compatibility['default_os_status']}")
    print(f"Version authority: {authority['version_manifest']}")
    if "generated_catalog_inventory" in authority:
        print(f"Generated catalog inventory: {authority['generated_catalog_inventory']}")
    if "generated_proof_registry" in authority:
        print(f"Generated proof registry: {authority['generated_proof_registry']}")
    if "project_adapter" in authority:
        print(f"Project adapter: {authority['project_adapter']}")
    if state.get("aide_lite"):
        aide_lite = state["aide_lite"]
        print(
            "AIDE Lite: "
            f"{aide_lite.get('status', 'unknown')} pin {str(aide_lite.get('admitted_commit', ''))[:7]} "
            f"(latest observed {str(aide_lite.get('latest_observed_head', ''))[:7]}, "
            f"DistributionManifest v1 {aide_lite.get('distribution_manifest_v1_status', 'unknown')})"
        )
    print(
        "Portable v2: "
        f"{portable_v2['status']} gate {portable_v2['gate']} "
        f"for {', '.join(portable_v2.get('porting_products', []))}"
    )
    if state.get("plasma_v2"):
        plasma_v2 = state["plasma_v2"]
        print(
            "Plasma v2: "
            f"{plasma_v2.get('status', 'unknown')} "
            f"(stable={plasma_v2.get('stable')}, release={plasma_v2.get('release_promotion')})"
        )
    print(
        "Plasma stable center: "
        f"{plasma['default_preset']} + {plasma['default_theme']}, "
        f"{plasma['minimum_kind']} -> {plasma['preferred_kind']}, {plasma['quality_class']}"
    )
    print(f"Plasma ship posture: {development['ship_posture']}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary", action="store_true", help="Print the current state summary after validation.")
    args = parser.parse_args()

    state = load_toml(STATE_PATH)
    errors = validate_state(state)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    if args.summary:
        print_summary(state)
    else:
        print("Project state checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
