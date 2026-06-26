"""Validate the portable v2 Gate C acceptance evidence."""

from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
CANDIDATE_BASE = ROOT / "validation" / "captures" / "portable-v2" / "candidate-base.json"
NATIVE_DIR = ROOT / "validation" / "captures" / "portable-v2" / "native-windows-current"
CAPTURE_DIR = ROOT / "validation" / "captures" / "portable-v2" / "gate-c"
OUT_DIR = ROOT / "out" / "gate-c" / "acceptance"

NOCTURNE_HASH = "5394a14b6622c17bfb10cd5721c08a4c92cdbddfb12f55c954ef1d5f6ef878b2"
RICOCHET_HASHES = {
    "0": "b8c7c0ea922be06b559485e22a16c286fd29e7496437cfb017a74aec68c6235a",
    "4": "7a1615e04d27e9dcb58da30b9520fcfa8dfe4b8fd5c14199750ef95754206ff1",
    "8": "a50582e8e8f5517eb2b56e7e01d5b0493b8432375a7e577b152801a4f6574b42",
    "32": "65c37015993c76632ce9e384db18b7cfa8c735049c66a72ed2833bbb45f70d25",
}
REQUIRED_CAPABILITIES = {
    "screensave.proof.nocturne.reference-v0.v1",
    "screensave.proof.nocturne.reference-v0.v2",
    "screensave.proof.ricochet.reference-v1.v1",
    "screensave.proof.ricochet.reference-v1.v2",
    "screensave.proof.portable-v2.equivalence",
    "screensave.bundle.portable-v2.equivalence",
}
FORBIDDEN_CAPABILITIES = {
    "screensave.command",
    "screensave.run",
    "screensave.exec",
    "screensave.proof.any",
    "screensave.proof.arbitrary-profile",
    "screensave.validate.arbitrary-command",
}
NATIVE_INPUT_PREFIXES = (
    "products/",
    "platform/",
    "build/msvc/",
    "build/mingw/",
    "catalog/products.toml",
    "catalog/generated/products_inventory.json",
    "catalog/artifact_profiles.toml",
    "catalog/artifact_sets.toml",
    "catalog/proof_profiles.toml",
    "tools/buildctl/",
)
PLASMA_RUNTIME_PREFIXES = (
    "products/savers/plasma/src/",
    "products/savers/plasma/include/",
)


def repo_path(path: pathlib.Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def git_names(args: list[str]) -> list[str]:
    text = git_text(args)
    if text == "unknown":
        return []
    return [line.strip().replace("\\", "/") for line in text.splitlines() if line.strip()]


def run(args: list[str], timeout_seconds: int = 240) -> dict[str, Any]:
    command = [sys.executable, *args]
    try:
        result = subprocess.run(
            command,
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout_seconds,
            check=False,
        )
        return {
            "command": command,
            "returncode": result.returncode,
            "stdout": result.stdout.strip(),
            "stderr": result.stderr.strip(),
            "status": "pass" if result.returncode == 0 else "fail",
            "timeout_seconds": timeout_seconds,
        }
    except subprocess.TimeoutExpired as exc:
        return {
            "command": command,
            "returncode": None,
            "stdout": (exc.stdout or "").strip() if isinstance(exc.stdout, str) else "",
            "stderr": (exc.stderr or "").strip() if isinstance(exc.stderr, str) else "",
            "status": "fail",
            "timeout_seconds": timeout_seconds,
            "failure_code": "timeout",
        }


def add_check(checks: list[dict[str, Any]], check_id: str, condition: bool, summary: str, **data: Any) -> None:
    checks.append(
        {
            "id": check_id,
            "status": "pass" if condition else "fail",
            "summary": summary,
            "data": data,
        }
    )


def add_command_check(checks: list[dict[str, Any]], check_id: str, args: list[str], timeout_seconds: int = 240) -> None:
    result = run(args, timeout_seconds=timeout_seconds)
    add_check(
        checks,
        check_id,
        result.get("returncode") == 0,
        "Command passed." if result.get("returncode") == 0 else "Command failed.",
        run=result,
    )


def native_inputs_changed_since(source_commit: str) -> list[str]:
    changed = git_names(["diff", "--name-only", f"{source_commit}..HEAD"])
    return [
        path
        for path in changed
        if any(path == prefix or path.startswith(prefix) for prefix in NATIVE_INPUT_PREFIXES)
    ]


def plasma_runtime_changes_since(source_commit: str) -> list[str]:
    changed = git_names(["diff", "--name-only", f"{source_commit}..HEAD"])
    return [
        path
        for path in changed
        if any(path.startswith(prefix) for prefix in PLASMA_RUNTIME_PREFIXES)
    ]


def scan_aide_runtime_refs() -> list[str]:
    roots = [
        ROOT / "products" / "savers" / "nocturne" / "src",
        ROOT / "products" / "savers" / "ricochet" / "src",
        ROOT / "platform" / "src",
        ROOT / "platform" / "include",
    ]
    hits: list[str] = []
    for root in roots:
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix.lower() not in {".c", ".h"}:
                continue
            text = path.read_text(encoding="utf-8", errors="ignore").lower()
            if "aide" in text:
                hits.append(repo_path(path))
    return hits


def v2_adapter_violations() -> list[str]:
    checks = [
        (
            ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_v2_adapter.c",
            "nocturne_core_",
            ["v1_session", "screensave_saver_environment", "nocturne_create_session", "nocturne_step_session", "nocturne_render_session"],
        ),
        (
            ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_v2_adapter.c",
            "ricochet_core_",
            ["v1_session", "screensave_saver_environment", "ricochet_create_session", "ricochet_step_session", "ricochet_render_session"],
        ),
    ]
    violations: list[str] = []
    for path, required, forbidden in checks:
        text = path.read_text(encoding="utf-8")
        if required not in text:
            violations.append(f"{repo_path(path)} missing {required}")
        for token in forbidden:
            if token in text:
                violations.append(f"{repo_path(path)} contains {token}")
    return violations


def build_report() -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    state = load_toml(STATE)
    candidate = load_json(CANDIDATE_BASE)
    artifact_summary = load_json(NATIVE_DIR / "artifact-summary.json")
    proof_summary = load_json(NATIVE_DIR / "proof-summary.json")
    pe_summary = load_json(NATIVE_DIR / "pe-audit-summary.json")

    portable_status = state.get("portable_v2", {}).get("status")
    accepted_mode = portable_status == "accepted"
    pre_acceptance_mode = portable_status == "active"
    add_check(
        checks,
        "project-state-portable-v2-mode",
        accepted_mode or pre_acceptance_mode,
        "PROJECT_STATE portable_v2 status is active before acceptance or accepted after acceptance.",
        status=portable_status,
    )
    add_check(
        checks,
        "proof-kernel-v1-complete",
        state.get("proof_kernel_v1", {}).get("status") == "complete" and state.get("proof_kernel_v1", {}).get("remaining", []) == [],
        "Proof Kernel v1 is complete with no remaining work.",
        status=state.get("proof_kernel_v1", {}).get("status"),
        remaining=state.get("proof_kernel_v1", {}).get("remaining", []),
    )
    if accepted_mode:
        add_check(
            checks,
            "project-state-accepted-fields",
            state.get("portable_v2", {}).get("accepted_by") == "check_gate_c_acceptance.py"
            and state.get("portable_v2", {}).get("opened_next") == "plasma-v2-reference-slice"
            and state.get("development", {}).get("active_program") == "plasma-v2-reference-slice"
            and state.get("portable_v2", {}).get("remaining", []) == [],
            "Accepted state records Gate C closure and opens Plasma v2.",
            portable_v2=state.get("portable_v2", {}),
            development=state.get("development", {}),
        )

    add_command_check(checks, "product-core-boundaries", ["tools/scripts/check_product_core_boundaries.py"])
    add_command_check(checks, "portable-v2-headers", ["tools/scripts/check_portable_v2.py"])
    add_command_check(checks, "portable-v2-equivalence", ["tools/scripts/check_portable_v2_equivalence.py"])
    add_command_check(checks, "libsslab-abi-v1", ["tools/scripts/check_sslab_abi_v1.py"])
    add_command_check(checks, "workbench-v2-inspect", ["tools/scripts/check_workbench_shell.py"])
    add_command_check(checks, "aide-evidence-export", ["tools/scripts/check_aide_evidence_bridge.py"])
    add_command_check(checks, "proof-bundle-v1", ["tools/scripts/check_proof_bundle_v1.py"], timeout_seconds=300)

    adapter_violations = v2_adapter_violations()
    add_check(
        checks,
        "v2-adapters-use-product-cores",
        not adapter_violations,
        "Nocturne and Ricochet v2 adapters call extracted product cores instead of v1 sessions.",
        violations=adapter_violations,
    )

    capabilities_run = run(["tools/project_adapter/screensave_project.py", "capabilities"])
    capabilities_payload: dict[str, Any] = {}
    try:
        capabilities_payload = json.loads(capabilities_run.get("stdout", "{}")).get("payload", {})
    except json.JSONDecodeError:
        capabilities_payload = {}
    capability_names = {item.get("name") for item in capabilities_payload.get("capabilities", [])}
    add_check(
        checks,
        "adapter-fixed-v2-capabilities",
        REQUIRED_CAPABILITIES <= capability_names,
        "Project adapter exposes the fixed portable v2 proof and bundle capabilities.",
        missing=sorted(REQUIRED_CAPABILITIES - capability_names),
    )
    forbidden_present = sorted(name for name in capability_names if name in FORBIDDEN_CAPABILITIES)
    add_check(
        checks,
        "adapter-no-generic-command-capability",
        not forbidden_present,
        "Project adapter does not expose generic command, run, exec, arbitrary-profile, or arbitrary-command capabilities.",
        forbidden_present=forbidden_present,
    )

    native_profile = next((item for item in artifact_summary.get("profiles", []) if item.get("profile") == "windows-current-x86"), {})
    tool_profile = next((item for item in artifact_summary.get("profiles", []) if item.get("profile") == "windows-current-tools"), {})
    add_check(
        checks,
        "native-artifact-summary",
        artifact_summary.get("status") == "pass"
        and native_profile.get("expected_count") == 19
        and native_profile.get("observed_count") == 19
        and tool_profile.get("expected_count") == 2
        and tool_profile.get("observed_count") == 2,
        "Native artifact summary records 19 current .scr artifacts and 2 current tools.",
        summary_ref=repo_path(NATIVE_DIR / "artifact-summary.json"),
    )
    pe_counts_ok = all(item.get("violation_count") == 0 for item in pe_summary.get("audits", []))
    pe_artifacts_ok = {item.get("profile"): item.get("artifact_count") for item in pe_summary.get("audits", [])} == {
        "windows-current-x86": 19,
        "windows-current-tools": 2,
    }
    add_check(
        checks,
        "native-pe-audit-summary",
        pe_summary.get("status") == "pass" and pe_counts_ok and pe_artifacts_ok,
        "Native PE audit summaries exist and report zero violations.",
        summary_ref=repo_path(NATIVE_DIR / "pe-audit-summary.json"),
    )

    proof_by_product = {(item.get("product"), item.get("abi"), item.get("path")): item for item in proof_summary.get("proofs", [])}
    ricochet_v2 = proof_by_product.get(("ricochet", "v1", "v2"), {})
    add_check(
        checks,
        "native-proof-summary",
        proof_summary.get("status") == "pass"
        and proof_by_product.get(("nocturne", "v0", "v1"), {}).get("hash") == NOCTURNE_HASH
        and proof_by_product.get(("nocturne", "v1", "v2"), {}).get("hash") == NOCTURNE_HASH
        and ricochet_v2.get("frames") == RICOCHET_HASHES,
        "Native proof summary records exact Nocturne and Ricochet v1/v2 hashes.",
        summary_ref=repo_path(NATIVE_DIR / "proof-summary.json"),
    )

    evidence_source = str(artifact_summary.get("source", {}).get("commit", ""))
    changed_native_inputs = native_inputs_changed_since(evidence_source) if evidence_source else ["missing evidence source"]
    add_check(
        checks,
        "native-evidence-current-for-runtime-inputs",
        not changed_native_inputs,
        "No native runtime, product, build, or catalog inputs changed after the native evidence source commit.",
        evidence_source=evidence_source,
        changed_native_inputs=changed_native_inputs,
    )

    bundle_path = ROOT / "out" / "proof" / "proof-bundle-v1" / "check-ricochet.json"
    bundle = load_json(bundle_path) if bundle_path.exists() else {}
    portable_axis = bundle.get("result_axes", {}).get("portable_v2_equivalence", {})
    add_check(
        checks,
        "proof-bundle-portable-v2-axis",
        portable_axis.get("status") == "pass"
        and portable_axis.get("products") == ["nocturne", "ricochet"]
        and portable_axis.get("profiles") == ["nocturne.reference.v0", "ricochet.reference.v1"]
        and portable_axis.get("claim_boundary") == "Named canary v1/v2 deterministic equivalence only.",
        "Proof Bundle v1 includes the explicit portable v2 equivalence axis.",
        bundle_ref=repo_path(bundle_path),
        axis=portable_axis,
    )

    aide_runtime_refs = scan_aide_runtime_refs()
    add_check(
        checks,
        "aide-absent-from-product-runtime",
        not aide_runtime_refs,
        "AIDE remains absent from product and platform runtime C sources.",
        refs=aide_runtime_refs,
    )

    candidate_source = candidate.get("source_commit", "b906edf5e81eb0386eb74596bc706b28d8c0e171")
    plasma_runtime_changes = plasma_runtime_changes_since(str(candidate_source))
    add_check(
        checks,
        "plasma-runtime-unchanged",
        not plasma_runtime_changes,
        "Plasma runtime files have not changed during Gate C closure.",
        candidate_source=candidate_source,
        changed_files=plasma_runtime_changes,
    )

    status = "pass" if all(item.get("status") == "pass" for item in checks) else "fail"
    return {
        "schema": "screensave-portable-v2-gate-c-acceptance-report-v0",
        "status": status,
        "mode": "accepted" if accepted_mode else "pre_acceptance",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "inputs": {
            "project_state": repo_path(STATE),
            "candidate_base": repo_path(CANDIDATE_BASE),
            "native_artifact_summary": repo_path(NATIVE_DIR / "artifact-summary.json"),
            "native_proof_summary": repo_path(NATIVE_DIR / "proof-summary.json"),
            "native_pe_audit_summary": repo_path(NATIVE_DIR / "pe-audit-summary.json"),
            "proof_bundle_check": repo_path(bundle_path),
        },
        "claim_boundary": "Gate C acceptance evidence only; not compatibility certification, artistic acceptance, release promotion, or Plasma runtime implementation.",
        "checks": checks,
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Portable v2 Gate C acceptance report",
        "",
        f"- Status: {report.get('status')}",
        f"- Mode: {report.get('mode')}",
        f"- Source commit: {report.get('source', {}).get('commit')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Checks",
        "",
    ]
    for check in report.get("checks", []):
        lines.append(f"- {check.get('status')}: {check.get('id')} - {check.get('summary')}")
    lines.append("")
    return "\n".join(lines)


def write_outputs(report: dict[str, Any], output_dir: pathlib.Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    inputs = {
        "schema": "screensave-portable-v2-gate-c-acceptance-inputs-v0",
        "status": report.get("status"),
        "mode": report.get("mode"),
        "source": report.get("source"),
        "inputs": report.get("inputs"),
    }
    (output_dir / "acceptance-inputs.json").write_text(json.dumps(inputs, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    (output_dir / "acceptance-report.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    (output_dir / "acceptance-report.md").write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--write-capture",
        action="store_true",
        help="Write the report to validation/captures/portable-v2/gate-c instead of out/gate-c/acceptance.",
    )
    args = parser.parse_args()
    report = build_report()
    write_outputs(report, CAPTURE_DIR if args.write_capture else OUT_DIR)
    print(f"Gate C acceptance {report['status']} ({report['mode']})")
    return 0 if report.get("status") == "pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())
