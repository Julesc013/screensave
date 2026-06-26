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
        "def command_validate",
        "def command_build",
        "def command_render",
        "def command_compare",
        "def command_audit",
        "def command_proof",
        "APPROVED_COMPARE_INPUT_ROOTS",
        "ARTIFACT_PROFILE_AUDIT_ROOTS",
        "def resolve_audit_profiles",
        "def blocked",
    ],
    ROOT / "contracts" / "project_adapter_v0.md": [
        "status",
        "capabilities",
        "catalog",
        "validate",
        "build",
        "render",
        "compare",
        "audit",
        "proof",
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
            status.get("payload", {}).get("active_program") == "portable-v2-seam",
            "status must report portable-v2-seam.",
            errors,
        )

        capabilities = run_adapter(["capabilities"])
        names = {item.get("name") for item in capabilities.get("payload", {}).get("capabilities", [])}
        require("screensave.project.status" in names, "capabilities must include screensave.project.status.", errors)
        require("screensave.project.capabilities" in names, "capabilities must include screensave.project.capabilities.", errors)
        require("screensave.catalog.read" in names, "capabilities must include screensave.catalog.read.", errors)
        require("screensave.validation.core" in names, "capabilities must include screensave.validation.core.", errors)
        require("screensave.build.windows-current-x86" in names, "capabilities must include screensave.build.windows-current-x86.", errors)
        require("screensave.build.windows-current-tools" in names, "capabilities must include screensave.build.windows-current-tools.", errors)
        require("screensave.proof.nocturne.render" in names, "capabilities must include screensave.proof.nocturne.render.", errors)
        require("screensave.proof.capture.compare" in names, "capabilities must include screensave.proof.capture.compare.", errors)
        require("screensave.artifact.pe.audit" in names, "capabilities must include screensave.artifact.pe.audit.", errors)
        require("screensave.proof.nocturne.exact" in names, "capabilities must include screensave.proof.nocturne.exact.", errors)
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

        validation = run_adapter(["validate"])
        require(validation.get("status") == "pass", "adapter validate must pass.", errors)

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

        proof = run_adapter(["proof", "--invocation-id", "check-proof"])
        require(proof.get("status") == "pass", "adapter proof must pass.", errors)
        require(proof.get("payload", {}).get("comparison_status") == "pass", "adapter proof comparison must pass.", errors)
        require(
            proof.get("payload", {}).get("pe_audit_artifact_profiles") == ["windows_current_x86_scr"],
            "adapter proof must include the selected PE audit artifact profile.",
            errors,
        )
        require(
            proof.get("payload", {}).get("pe_audit_claim_boundary") == "binary facts only; not compatibility certification",
            "adapter proof PE audit facts must preserve the compatibility claim boundary.",
            errors,
        )
        require(
            isinstance(proof.get("payload", {}).get("pe_audit_violation_count"), int),
            "adapter proof must include a numeric PE audit violation count.",
            errors,
        )
        require(
            proof.get("payload", {}).get("pe_audit_artifact_count", 0) > 0,
            "adapter proof must include a nonzero PE artifact count.",
            errors,
        )
        require(
            proof.get("payload", {}).get("pe_audit_status") == "informational",
            "adapter proof must preserve the structured PE audit status.",
            errors,
        )
        require(
            proof.get("payload", {}).get("pe_audit_missing_inputs") == [],
            "adapter proof must report no missing PE audit inputs for the current profile.",
            errors,
        )
        proof_dir = repo_ref_to_path(proof.get("payload", {}).get("output_dir", ""))
        require((proof_dir / "adapter-proof.json").exists(), "adapter proof must write adapter-proof.json.", errors)
        require((proof_dir / "artifact-manifest.json").exists(), "adapter proof must write artifact-manifest.json.", errors)
        require(
            repo_ref_to_path(proof.get("payload", {}).get("pe_audit_ref", "")).exists(),
            "adapter proof must write a PE audit report.",
            errors,
        )
        require(
            repo_ref_to_path(proof.get("payload", {}).get("pe_audit_json_ref", "")).exists(),
            "adapter proof must write a PE audit JSON report.",
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
