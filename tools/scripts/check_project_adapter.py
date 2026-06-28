"""Validate the ScreenSave project adapter command surface."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
ADAPTER = ROOT / "tools" / "project_adapter" / "screensave_project.py"
CAPABILITY_BINDINGS = ROOT / "tools" / "project_adapter" / "capability_bindings.json"
RECEIPT_SCHEMAS = ROOT / "tools" / "project_adapter" / "receipt_schemas.json"
ARTIFACT_PROFILE_AUDIT_ROOTS = ROOT / "tools" / "project_adapter" / "artifact_profile_audit_roots.json"
ARTIFACT_PROFILES = ROOT / "catalog" / "artifact_profiles.toml"
PE_AUDIT = ROOT / "tools" / "scripts" / "audit_pe_artifacts.py"
BUILDCTL = ROOT / "tools" / "buildctl" / "screensave_build.py"

REQUIRED_PATHS = [
    ROOT / "contracts" / "project_adapter_v0.md",
    ROOT / "tools" / "project_adapter" / "README.md",
    ARTIFACT_PROFILES,
    CAPABILITY_BINDINGS,
    RECEIPT_SCHEMAS,
    ARTIFACT_PROFILE_AUDIT_ROOTS,
    PE_AUDIT,
    BUILDCTL,
    ADAPTER,
]

REQUIRED_TEXT = {
    ADAPTER: [
        "screensave-project-adapter-v0",
        "def command_status",
        "def command_capabilities",
        "def command_catalog",
        "def command_profiles",
        "def command_validate",
        "VALIDATION_TIERS",
        "ADMITTED_PROOF_PROFILES",
        "def command_build",
        "def command_render",
        "def command_compare",
        "def command_audit",
        "def command_proof",
        "def command_bundle",
        "def command_equivalence",
        "APPROVED_COMPARE_INPUT_ROOTS",
        "ARTIFACT_PROFILE_AUDIT_ROOTS",
        "def resolve_audit_profiles",
        "def blocked",
    ],
    ROOT / "contracts" / "project_adapter_v0.md": [
        "status",
        "capabilities",
        "catalog",
        "profiles",
        "validate",
        "build",
        "render",
        "compare",
        "audit",
        "proof",
        "bundle",
        "equivalence",
        "AIDE may consume receipts",
        "tools/project_adapter/capability_bindings.json",
        "tools/project_adapter/receipt_schemas.json",
        "tools/project_adapter/artifact_profile_audit_roots.json",
        "tools/buildctl/screensave_build.py",
        "out/aide/screensave-project-adapter/invocations/",
        "accept arbitrary output paths",
        "arbitrary compiler, linker, or MSBuild arguments",
    ],
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_adapter(args: list[str]) -> dict:
    output = subprocess.check_output([sys.executable, str(ADAPTER), *args], cwd=ROOT, text=True)
    return json.loads(output)


def run_adapter_result(args: list[str]) -> tuple[int, dict]:
    result = subprocess.run(
        [sys.executable, str(ADAPTER), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError:
        payload = {"stdout": result.stdout, "stderr": result.stderr}
    return result.returncode, payload


def repo_ref_to_path(value: str) -> pathlib.Path:
    path = pathlib.Path(value)
    if path.is_absolute():
        return path
    return ROOT / path


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing project adapter path: {path.relative_to(ROOT)}", errors)

    for path, needles in REQUIRED_TEXT.items():
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        for needle in needles:
            require(needle in text, f"{path.relative_to(ROOT)} is missing expected text: {needle!r}", errors)

    if not errors:
        status = run_adapter(["status"])
        require(status.get("adapter_schema") == "screensave-project-adapter-v0", "status must emit adapter schema.", errors)
        require(
            status.get("payload", {}).get("active_program") in {
                "portable-v2-seam",
                "plasma-v2-reference-slice",
                "plasma-v2-release-readiness",
                "plasma-v2-release-candidate",
                "plasma-v2-stable-promotion",
                "plasma-v2-stable-repair",
                "plasma-v2-instrument-repair",
                "plasma-v2-publication-prep",
                "plasma-v2-publication",
            },
            "status must report the active portable-v2 or Plasma v2 program.",
            errors,
        )

        capabilities = run_adapter(["capabilities"])
        names = {item.get("name") for item in capabilities.get("payload", {}).get("capabilities", [])}
        require("screensave.project.status" in names, "capabilities must include screensave.project.status.", errors)
        require("screensave.project.capabilities" in names, "capabilities must include screensave.project.capabilities.", errors)
        require("screensave.catalog.read" in names, "capabilities must include screensave.catalog.read.", errors)
        require("screensave.profiles.read" in names, "capabilities must include screensave.profiles.read.", errors)
        require("screensave.validation.t0" in names, "capabilities must include screensave.validation.t0.", errors)
        require("screensave.validation.t1" in names, "capabilities must include screensave.validation.t1.", errors)
        require("screensave.validation.t2" in names, "capabilities must include screensave.validation.t2.", errors)
        require("screensave.validation.t3" in names, "capabilities must include screensave.validation.t3.", errors)
        require("screensave.build.windows-current-x86" in names, "capabilities must include screensave.build.windows-current-x86.", errors)
        require("screensave.build.windows-current-tools" in names, "capabilities must include screensave.build.windows-current-tools.", errors)
        require("screensave.proof.nocturne.render" in names, "capabilities must include screensave.proof.nocturne.render.", errors)
        require("screensave.proof.capture.compare" in names, "capabilities must include screensave.proof.capture.compare.", errors)
        require("screensave.artifact.pe.audit" in names, "capabilities must include screensave.artifact.pe.audit.", errors)
        require("screensave.proof.nocturne.reference-v0.v1" in names, "capabilities must include screensave.proof.nocturne.reference-v0.v1.", errors)
        require("screensave.proof.nocturne.reference-v0.v2" in names, "capabilities must include screensave.proof.nocturne.reference-v0.v2.", errors)
        require("screensave.proof.ricochet.reference-v1.v1" in names, "capabilities must include screensave.proof.ricochet.reference-v1.v1.", errors)
        require("screensave.proof.ricochet.reference-v1.v2" in names, "capabilities must include screensave.proof.ricochet.reference-v1.v2.", errors)
        require("screensave.proof.plasma-v2.reference-preview.v2" in names, "capabilities must include screensave.proof.plasma-v2.reference-preview.v2.", errors)
        require("screensave.proof.portable-v2.equivalence" in names, "capabilities must include screensave.proof.portable-v2.equivalence.", errors)
        require("screensave.bundle.portable-v2.equivalence" in names, "capabilities must include screensave.bundle.portable-v2.equivalence.", errors)
        require("screensave.bundle.nocturne.reference-v0.v1" in names, "capabilities must include screensave.bundle.nocturne.reference-v0.v1.", errors)
        require("screensave.bundle.nocturne.reference-v0.v2" in names, "capabilities must include screensave.bundle.nocturne.reference-v0.v2.", errors)
        require("screensave.bundle.ricochet.reference-v1.v1" in names, "capabilities must include screensave.bundle.ricochet.reference-v1.v1.", errors)
        require("screensave.bundle.ricochet.reference-v1.v2" in names, "capabilities must include screensave.bundle.ricochet.reference-v1.v2.", errors)
        require("screensave.bundle.plasma-v2.reference-preview.v2" in names, "capabilities must include screensave.bundle.plasma-v2.reference-preview.v2.", errors)
        require("screensave.plasma.v2.stable-candidate.check" in names, "capabilities must include screensave.plasma.v2.stable-candidate.check.", errors)
        require("screensave.plasma.v2.package-stage.check" in names, "capabilities must include screensave.plasma.v2.package-stage.check.", errors)
        require("screensave.plasma.v2.manager-preview.check" in names, "capabilities must include screensave.plasma.v2.manager-preview.check.", errors)
        require("screensave.plasma.v2.release-readiness.check" in names, "capabilities must include screensave.plasma.v2.release-readiness.check.", errors)
        require("screensave.plasma.v2.release-candidate.check" in names, "capabilities must include screensave.plasma.v2.release-candidate.check.", errors)
        require("screensave.plasma.v2.release-candidate.package" in names, "capabilities must include screensave.plasma.v2.release-candidate.package.", errors)
        require("screensave.plasma.v2.release-candidate.evidence" in names, "capabilities must include screensave.plasma.v2.release-candidate.evidence.", errors)
        require("screensave.plasma.v2.release-candidate.review" in names, "capabilities must include screensave.plasma.v2.release-candidate.review.", errors)
        require("screensave.plasma.v2.stable-promotion.check" in names, "capabilities must include screensave.plasma.v2.stable-promotion.check.", errors)
        require("screensave.plasma.v2.stable-promotion.proof" in names, "capabilities must include screensave.plasma.v2.stable-promotion.proof.", errors)
        require("screensave.plasma.v2.stable-promotion.package" in names, "capabilities must include screensave.plasma.v2.stable-promotion.package.", errors)
        require("screensave.plasma.v2.stable-promotion.support" in names, "capabilities must include screensave.plasma.v2.stable-promotion.support.", errors)
        require("screensave.plasma.v2.stable-promotion.security" in names, "capabilities must include screensave.plasma.v2.stable-promotion.security.", errors)
        require("screensave.plasma.v2.stable-promotion.review" in names, "capabilities must include screensave.plasma.v2.stable-promotion.review.", errors)
        require("screensave.aide.workunits.check" in names, "capabilities must include screensave.aide.workunits.check.", errors)
        require("screensave.aide.repairs.check" in names, "capabilities must include screensave.aide.repairs.check.", errors)
        require("screensave.aide.agentic.check" in names, "capabilities must include screensave.aide.agentic.check.", errors)
        for forbidden_capability in (
            "screensave.command",
            "screensave.run",
            "screensave.exec",
            "screensave.release.publish",
            "screensave.promote.stable",
            "screensave.promote.stable.unchecked",
            "screensave.agent.apply",
        ):
            require(forbidden_capability not in names, f"capabilities must not include {forbidden_capability}.", errors)
        require(
            capabilities.get("payload", {}).get("output_root") == "out/aide/screensave-project-adapter/invocations",
            "capabilities must expose the contained invocation output root.",
            errors,
        )
        require(
            capabilities.get("payload", {}).get("artifact_profile_audit_roots_ref") == "tools/project_adapter/artifact_profile_audit_roots.json",
            "capabilities must expose artifact-profile audit roots.",
            errors,
        )

        bindings = json.loads(CAPABILITY_BINDINGS.read_text(encoding="utf-8"))
        schemas = json.loads(RECEIPT_SCHEMAS.read_text(encoding="utf-8"))
        audit_roots = json.loads(ARTIFACT_PROFILE_AUDIT_ROOTS.read_text(encoding="utf-8"))
        with ARTIFACT_PROFILES.open("rb") as handle:
            catalog_profiles = tomllib.load(handle)
        catalog_profile_keys = {item.get("key") for item in catalog_profiles.get("artifact_profiles", [])}
        audit_profile_keys = {item.get("key") for item in audit_roots.get("profiles", [])}
        require(audit_profile_keys <= catalog_profile_keys, "adapter audit roots must reference catalog artifact profiles.", errors)
        for item in audit_roots.get("profiles", []):
            for root_value in item.get("roots", []):
                require(
                    str(root_value).replace("\\", "/").startswith("out/"),
                    f"adapter audit root must stay under out/: {root_value}",
                    errors,
                )
        binding_names = {item.get("name") for item in bindings.get("capabilities", [])}
        require(names == binding_names, "adapter capabilities must match capability_bindings.json.", errors)
        require(
            bindings.get("output_root") == "out/aide/screensave-project-adapter/invocations",
            "capability bindings must use the out/aide invocation root.",
            errors,
        )
        require(
            "windows_current_x86_scr" in set(audit_roots.get("default_profiles", [])),
            "artifact profile audit roots must include windows_current_x86_scr by default.",
            errors,
        )
        schema_commands = set(schemas.get("commands", {}))
        for item in bindings.get("capabilities", []):
            decoder_schema = str(item.get("decoder_schema", ""))
            require(
                decoder_schema.startswith("tools/project_adapter/receipt_schemas.json#/commands/"),
                f"{item.get('name')} must point to the adapter receipt schema file.",
                errors,
            )
            command_name = decoder_schema.rsplit("/", 1)[-1]
            require(command_name in schema_commands, f"{item.get('name')} points to an unknown receipt schema.", errors)

        catalog = run_adapter(["catalog"])
        require(catalog.get("payload", {}).get("saver_count", 0) >= 19, "adapter catalog must report current saver family.", errors)

        profiles = run_adapter(["profiles"])
        admitted_profiles = {item.get("key") for item in profiles.get("payload", {}).get("admitted_profiles", [])}
        require(
            admitted_profiles == {"nocturne.reference.v0", "ricochet.reference.v1", "plasma.v2.reference.preview"},
            "adapter profiles must expose only fixed Nocturne, Ricochet, and Plasma preview proof profiles.",
            errors,
        )
        require(
            "screensave.proof.any-profile" in profiles.get("payload", {}).get("refusals", []),
            "adapter profiles must explicitly refuse any-profile proof capability.",
            errors,
        )

        validation = run_adapter(["validate"])
        require(validation.get("status") == "pass", "adapter validate must pass.", errors)
        require(validation.get("payload", {}).get("tier") == "T0", "adapter validate default tier must be T0.", errors)
        require(
            validation.get("payload", {}).get("command_binding", {}).get("capability") == "screensave.validation.t0",
            "adapter validate default command binding must report screensave.validation.t0.",
            errors,
        )

        build = run_adapter(["build", "--invocation-id", "check-build", "--profile", "windows-current-x86", "--dry-run"])
        require(build.get("status") == "informational", "adapter build dry-run must be informational.", errors)
        require(build.get("payload", {}).get("capability") == "screensave.build.windows-current-x86", "adapter build must report the selected fixed build capability.", errors)
        require(build.get("payload", {}).get("dry_run") is True, "adapter build dry-run must preserve dry_run true.", errors)
        require(build.get("payload", {}).get("build_receipt_status") == "informational", "adapter build must preserve build receipt status.", errors)
        require(
            repo_ref_to_path(build.get("payload", {}).get("build_receipt_ref", "")).exists(),
            "adapter build dry-run must write build-receipt.json.",
            errors,
        )
        require(
            repo_ref_to_path(build.get("payload", {}).get("artifact_manifest_ref", "")).exists(),
            "adapter build dry-run must write an adapter artifact-manifest.json.",
            errors,
        )

        render = run_adapter(["render", "--invocation-id", "check-render"])
        require(render.get("status") == "pass", "adapter render must pass.", errors)
        render_payload = render.get("payload", {})
        capture_path = repo_ref_to_path(render_payload.get("capture_ref", ""))
        manifest_path = repo_ref_to_path(render_payload.get("artifact_manifest_ref", ""))
        require(capture_path.exists(), "adapter render must write capture.ppm.", errors)
        require(manifest_path.exists(), "adapter render must write artifact-manifest.json.", errors)
        require(
            "out/aide/screensave-project-adapter/invocations/render/check-render" in render_payload.get("output_dir", ""),
            "adapter render must use a contained invocation root.",
            errors,
        )

        compare = run_adapter(["compare", "--invocation-id", "check-compare", "--actual", str(capture_path)])
        require(compare.get("status") == "pass", "adapter compare must pass.", errors)
        require(compare.get("payload", {}).get("comparison_status") == "pass", "adapter compare comparison_status must pass.", errors)
        require(
            repo_ref_to_path(compare.get("payload", {}).get("artifact_manifest_ref", "")).exists(),
            "adapter compare must write artifact-manifest.json.",
            errors,
        )

        audit = run_adapter(["audit", "--invocation-id", "check-audit", "--artifact-profile", "windows_current_x86_scr"])
        require(audit.get("status") == "informational", "adapter audit must report informational facts by default.", errors)
        require(repo_ref_to_path(audit.get("payload", {}).get("audit_ref", "")).exists(), "adapter audit must write a report.", errors)
        require(repo_ref_to_path(audit.get("payload", {}).get("audit_json_ref", "")).exists(), "adapter audit must write a JSON report.", errors)
        require(
            audit.get("payload", {}).get("artifact_profiles") == ["windows_current_x86_scr"],
            "adapter audit must report selected artifact profiles.",
            errors,
        )
        require(audit.get("payload", {}).get("artifact_count", 0) > 0, "adapter audit must report a nonzero artifact count.", errors)
        require(audit.get("payload", {}).get("audit_status") == "informational", "adapter audit must preserve PE audit status.", errors)
        require(audit.get("payload", {}).get("missing_inputs") == [], "adapter audit must report no missing inputs for the current profile.", errors)

        proof = run_adapter(["proof", "--invocation-id", "check-proof-nocturne", "--profile", "nocturne.reference.v0"])
        require(proof.get("status") == "pass", "adapter proof must pass.", errors)
        require(
            proof.get("payload", {}).get("capability") == "screensave.proof.nocturne.reference-v0.v1",
            "adapter proof must report the Nocturne fixed profile capability.",
            errors,
        )
        require(proof.get("payload", {}).get("execution_path") == "v1", "adapter proof default path must be v1.", errors)
        require(proof.get("payload", {}).get("profile_proof_status") == "pass", "adapter proof profile status must pass.", errors)
        require(proof.get("payload", {}).get("comparison_status") == "pass", "adapter proof comparison must pass.", errors)
        require(proof.get("payload", {}).get("lifecycle_status") == "pass", "adapter proof lifecycle must pass.", errors)
        require(
            "artistic acceptance" in str(proof.get("payload", {}).get("claim_boundary", "")),
            "adapter proof must preserve the proof claim boundary.",
            errors,
        )
        proof_dir = repo_ref_to_path(proof.get("payload", {}).get("output_dir", ""))
        require((proof_dir / "adapter-proof.json").exists(), "adapter proof must write adapter-proof.json.", errors)
        require((proof_dir / "artifact-manifest.json").exists(), "adapter proof must write artifact-manifest.json.", errors)
        require(
            repo_ref_to_path(proof.get("payload", {}).get("profile_proof_ref", "")).exists(),
            "adapter proof must write profile-proof.json.",
            errors,
        )

        ricochet_proof = run_adapter(["proof", "--invocation-id", "check-proof-ricochet", "--profile", "ricochet.reference.v1"])
        require(ricochet_proof.get("status") == "pass", "adapter Ricochet proof must pass.", errors)
        require(
            ricochet_proof.get("payload", {}).get("capability") == "screensave.proof.ricochet.reference-v1.v1",
            "adapter Ricochet proof must report the fixed profile capability.",
            errors,
        )

        ricochet_v2_proof = run_adapter(["proof", "--invocation-id", "check-proof-ricochet-v2", "--profile", "ricochet.reference.v1", "--path", "v2"])
        require(ricochet_v2_proof.get("status") == "pass", "adapter Ricochet v2 proof must pass.", errors)
        require(
            ricochet_v2_proof.get("payload", {}).get("capability") == "screensave.proof.ricochet.reference-v1.v2",
            "adapter Ricochet v2 proof must report the fixed v2 profile capability.",
            errors,
        )

        plasma_v2_proof = run_adapter(["proof", "--invocation-id", "check-proof-plasma-v2", "--profile", "plasma.v2.reference.preview", "--path", "v2"])
        require(plasma_v2_proof.get("status") == "pass", "adapter Plasma v2 proof must pass.", errors)
        require(
            plasma_v2_proof.get("payload", {}).get("capability") == "screensave.proof.plasma-v2.reference-preview.v2",
            "adapter Plasma v2 proof must report the fixed preview profile capability.",
            errors,
        )
        require(
            plasma_v2_proof.get("payload", {}).get("claim_boundary", "").find("not stable promotion") >= 0,
            "adapter Plasma v2 proof must preserve the preview-only claim boundary.",
            errors,
        )

        bundle = run_adapter(["bundle", "--invocation-id", "check-bundle-ricochet", "--profile", "ricochet.reference.v1", "--path", "v2"])
        require(bundle.get("status") == "pass", "adapter Ricochet bundle must pass.", errors)
        require(
            bundle.get("payload", {}).get("capability") == "screensave.bundle.ricochet.reference-v1.v2",
            "adapter bundle must report the fixed Ricochet v2 bundle capability.",
            errors,
        )
        require(bundle.get("payload", {}).get("execution_path") == "v2", "adapter bundle must record v2 path.", errors)
        require(
            repo_ref_to_path(bundle.get("payload", {}).get("proof_bundle_ref", "")).exists(),
            "adapter bundle must write proof-bundle-v1.json.",
            errors,
        )

        plasma_bundle = run_adapter(["bundle", "--invocation-id", "check-bundle-plasma-v2", "--profile", "plasma.v2.reference.preview", "--path", "v2"])
        require(plasma_bundle.get("status") == "pass", "adapter Plasma v2 bundle must pass.", errors)
        require(
            plasma_bundle.get("payload", {}).get("capability") == "screensave.bundle.plasma-v2.reference-preview.v2",
            "adapter Plasma v2 bundle must report the fixed preview bundle capability.",
            errors,
        )
        require(plasma_bundle.get("payload", {}).get("execution_path") == "v2", "adapter Plasma v2 bundle must record v2 path.", errors)
        require(
            repo_ref_to_path(plasma_bundle.get("payload", {}).get("proof_bundle_ref", "")).exists(),
            "adapter Plasma v2 bundle must write proof-bundle-v1.json.",
            errors,
        )

        equivalence = run_adapter(["equivalence", "--invocation-id", "check-portable-v2-equivalence"])
        require(equivalence.get("status") == "pass", "adapter portable v2 equivalence must pass.", errors)
        require(
            equivalence.get("payload", {}).get("capability") == "screensave.proof.portable-v2.equivalence",
            "adapter equivalence must report the fixed equivalence capability.",
            errors,
        )
        require(
            repo_ref_to_path(equivalence.get("payload", {}).get("equivalence_ref", "")).exists(),
            "adapter equivalence must write portable-v2-equivalence.json.",
            errors,
        )

        missing_audit_json = ROOT / "out" / "aide" / "screensave-project-adapter" / "check-missing-pe-audit.json"
        missing_audit_txt = ROOT / "out" / "aide" / "screensave-project-adapter" / "check-missing-pe-audit.txt"
        missing_audit = subprocess.run(
            [
                sys.executable,
                str(PE_AUDIT),
                "--output",
                str(missing_audit_txt),
                "--json-output",
                str(missing_audit_json),
                "out/definitely-missing-pe-audit-root",
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        missing_payload = json.loads(missing_audit_json.read_text(encoding="utf-8")) if missing_audit_json.exists() else {}
        require(missing_audit.returncode != 0, "PE audit must fail closed for a missing artifact root.", errors)
        require(missing_payload.get("status") == "blocked", "missing PE audit root must produce blocked status.", errors)
        require(missing_payload.get("artifact_count") == 0, "missing PE audit root must report zero artifacts.", errors)

        refusal_code, refusal = run_adapter_result(["render", "--invocation-id", "..\\escape"])
        require(refusal_code != 0, "invalid invocation ids must be refused.", errors)
        require(refusal.get("status") == "blocked", "invalid invocation ids must return blocked status.", errors)
        require(
            refusal.get("payload", {}).get("refusal", {}).get("code") == "invalid_invocation_id",
            "invalid invocation ids must return invalid_invocation_id.",
            errors,
        )

        outside_code, outside = run_adapter_result(["compare", "--actual", str(ROOT / "README.md")])
        require(outside_code != 0, "compare must refuse inputs outside approved roots.", errors)
        require(outside.get("status") == "blocked", "outside compare input must return blocked status.", errors)
        require(
            outside.get("payload", {}).get("refusal", {}).get("code") == "input_root_denied",
            "outside compare input must return input_root_denied.",
            errors,
        )

        profile_code, profile_refusal = run_adapter_result(["audit", "--artifact-profile", "source_sdk"])
        require(profile_code != 0, "audit must refuse non-admitted artifact profiles.", errors)
        require(profile_refusal.get("status") == "blocked", "unknown audit profile must return blocked status.", errors)
        require(
            profile_refusal.get("payload", {}).get("refusal", {}).get("code") == "unknown_artifact_profile",
            "unknown audit profile must return unknown_artifact_profile.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Project adapter checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
