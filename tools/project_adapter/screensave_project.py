"""ScreenSave project-owned adapter for proof-native coordination."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE_PATH = ROOT / "PROJECT_STATE.toml"
VERSION_PATH = ROOT / "VERSION.toml"
SSLAB = ROOT / "tools" / "sslab" / "sslab.py"
GENERATED_INVENTORY = ROOT / "catalog" / "generated" / "products_inventory.json"
PE_AUDIT = ROOT / "tools" / "scripts" / "audit_pe_artifacts.py"
BUILDCTL = ROOT / "tools" / "buildctl" / "screensave_build.py"
CAPABILITY_BINDINGS = ROOT / "tools" / "project_adapter" / "capability_bindings.json"
RECEIPT_SCHEMAS = ROOT / "tools" / "project_adapter" / "receipt_schemas.json"
ARTIFACT_PROFILE_AUDIT_ROOTS = ROOT / "tools" / "project_adapter" / "artifact_profile_audit_roots.json"
ADAPTER_OUTPUT_ROOT = ROOT / "out" / "aide" / "screensave-project-adapter"
INVOCATION_OUTPUT_ROOT = ADAPTER_OUTPUT_ROOT / "invocations"
COMMITTED_CANARY = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne" / "capture.ppm"
APPROVED_COMPARE_INPUT_ROOTS = [
    ROOT / "validation" / "captures",
    ADAPTER_OUTPUT_ROOT,
]
INVOCATION_ID_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]{0,79}$")
MAX_RENDER_WIDTH = 512
MAX_RENDER_HEIGHT = 512
MAX_RENDER_FRAMES = 300
MAX_RENDER_DELTA_MS = 1000
MAX_CAPTURE_BYTES = 50 * 1024 * 1024
MAX_AUDIT_PROFILES = 8

BUILD_CAPABILITIES = {
    "windows-current-x86": "screensave.build.windows-current-x86",
    "windows-current-tools": "screensave.build.windows-current-tools",
}

CORE_VALIDATION = [
    ["tools/scripts/check_project_state.py"],
    ["tools/scripts/check_catalog_profiles.py"],
    ["tools/scripts/check_catalog_generated.py"],
    ["tools/scripts/check_contracts.py"],
    ["tools/scripts/check_proof_kernel.py"],
    ["tools/scripts/check_docs_basics.py"],
]


class AdapterError(Exception):
    def __init__(self, code: str, detail: str, payload: dict[str, Any] | None = None) -> None:
        super().__init__(detail)
        self.code = code
        self.detail = detail
        self.payload = payload or {}


def repo_path(path: pathlib.Path) -> str:
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def is_within(path: pathlib.Path, root: pathlib.Path) -> bool:
    try:
        path.resolve().relative_to(root.resolve())
        return True
    except ValueError:
        return False


def safe_invocation_id(value: str) -> str:
    if not INVOCATION_ID_RE.match(value) or ".." in value:
        raise AdapterError(
            "invalid_invocation_id",
            "Invocation ids must be 1-80 ASCII letters, digits, dots, dashes, or underscores and must not contain '..'.",
            {"invocation_id": value},
        )
    return value


def invocation_dir(command: str, invocation_id: str) -> pathlib.Path:
    safe_id = safe_invocation_id(invocation_id)
    output_dir = (INVOCATION_OUTPUT_ROOT / command / safe_id).resolve()
    if not is_within(output_dir, INVOCATION_OUTPUT_ROOT):
        raise AdapterError("output_root_escape", "Invocation output root must stay under the adapter output root.")
    output_dir.mkdir(parents=True, exist_ok=True)
    return output_dir


def display_roots(roots: list[pathlib.Path]) -> list[str]:
    return [repo_path(root) for root in roots]


def resolve_input_file(label: str, value: str, allowed_roots: list[pathlib.Path]) -> pathlib.Path:
    path = pathlib.Path(value)
    if not path.is_absolute():
        path = ROOT / path
    path = path.resolve()
    if not path.exists():
        raise AdapterError("input_not_found", f"{label} does not exist.", {label: str(value)})
    if not path.is_file():
        raise AdapterError("input_not_file", f"{label} must be a file.", {label: repo_path(path)})
    if not any(is_within(path, root) for root in allowed_roots):
        raise AdapterError(
            "input_root_denied",
            f"{label} must be under an approved input root.",
            {label: repo_path(path), "approved_roots": display_roots(allowed_roots)},
        )
    if path.stat().st_size > MAX_CAPTURE_BYTES:
        raise AdapterError(
            "quota_exceeded",
            f"{label} exceeds the adapter capture-size quota.",
            {label: repo_path(path), "max_capture_bytes": MAX_CAPTURE_BYTES},
        )
    return path


def load_audit_profile_roots() -> dict[str, Any]:
    return load_json(ARTIFACT_PROFILE_AUDIT_ROOTS)


def resolve_audit_profiles(values: list[str] | None) -> tuple[list[str], list[pathlib.Path]]:
    profile_roots = load_audit_profile_roots()
    profile_map = {item.get("key"): item for item in profile_roots.get("profiles", [])}
    selected = list(values) if values else list(profile_roots.get("default_profiles", []))

    if len(selected) > MAX_AUDIT_PROFILES:
        raise AdapterError(
            "quota_exceeded",
            "Too many artifact profiles were requested for PE audit.",
            {"requested": len(selected), "max_artifact_profiles": MAX_AUDIT_PROFILES},
        )

    roots: list[pathlib.Path] = []
    for key in selected:
        profile = profile_map.get(key)
        if profile is None:
            raise AdapterError(
                "unknown_artifact_profile",
                "PE audit artifact profile is not admitted by the ScreenSave adapter audit-root map.",
                {"artifact_profile": key, "admitted_profiles": sorted(profile_map)},
            )
        for value in profile.get("roots", []):
            path = (ROOT / value).resolve()
            if not is_within(path, ROOT / "out"):
                raise AdapterError(
                    "input_root_denied",
                    "PE audit roots must stay under out/.",
                    {"artifact_profile": key, "root": repo_path(path)},
                )
            roots.append(path)
    return selected, roots


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def source_payload() -> dict[str, Any]:
    return {
        "commit": git_text(["rev-parse", "HEAD"]),
        "branch": git_text(["branch", "--show-current"]),
        "dirty": bool(git_text(["status", "--short"])),
    }


def envelope(command: str, status: str, payload: dict[str, Any]) -> dict[str, Any]:
    return {
        "adapter_schema": "screensave-project-adapter-v0",
        "project": "screensave",
        "command": command,
        "status": status,
        "source": source_payload(),
        "payload": payload,
        "limits": [
            "Adapter delegates to ScreenSave-owned validators and proof tools.",
            "Adapter does not edit, merge, publish, certify compatibility, or accept artistic quality.",
            "AIDE may consume receipts but must not define ScreenSave product truth.",
        ],
    }


def write_json(result: dict[str, Any]) -> None:
    print(json.dumps(result, indent=2, sort_keys=True))


def exit_code_for_status(status: str) -> int:
    return 0 if status in {"pass", "informational"} else 1


def blocked(command: str, error: AdapterError) -> int:
    payload = dict(error.payload)
    payload["refusal"] = {
        "code": error.code,
        "detail": error.detail,
    }
    write_json(envelope(command, "blocked", payload))
    return 1


def capability_binding_for_command(command: str, capability_name: str | None = None) -> dict[str, Any]:
    bindings = load_json(CAPABILITY_BINDINGS)
    for item in bindings.get("capabilities", []):
        if capability_name is not None and item.get("name") == capability_name:
            return item
        if item.get("command") == command:
            return item
    return {}


def command_binding_payload(
    command: str,
    argv: list[str],
    delegated_tools: list[pathlib.Path],
    capability_name: str | None = None,
) -> dict[str, Any]:
    return {
        "capability": capability_binding_for_command(command, capability_name).get("name"),
        "argv": argv,
        "adapter_sha256": sha256_file(pathlib.Path(__file__).resolve()),
        "capability_binding_ref": repo_path(CAPABILITY_BINDINGS),
        "capability_binding_sha256": sha256_file(CAPABILITY_BINDINGS),
        "receipt_schema_ref": repo_path(RECEIPT_SCHEMAS),
        "receipt_schema_sha256": sha256_file(RECEIPT_SCHEMAS),
        "delegated_tools": [
            {
                "path": repo_path(tool),
                "sha256": sha256_file(tool),
            }
            for tool in delegated_tools
        ],
    }


def artifact_fact(label: str, path: pathlib.Path, kind: str) -> dict[str, Any]:
    resolved = path.resolve()
    if not resolved.exists():
        return {
            "label": label,
            "kind": kind,
            "path": repo_path(resolved),
            "exists": False,
        }
    return {
        "label": label,
        "kind": kind,
        "path": repo_path(resolved),
        "exists": True,
        "bytes": resolved.stat().st_size,
        "sha256": sha256_file(resolved),
    }


def write_artifact_manifest(output_dir: pathlib.Path, command: str, artifacts: list[tuple[str, pathlib.Path, str]]) -> pathlib.Path:
    manifest_path = output_dir / "artifact-manifest.json"
    manifest = {
        "manifest_schema": "screensave-project-adapter-artifact-manifest-v0",
        "command": command,
        "output_root": repo_path(output_dir),
        "source": source_payload(),
        "artifacts": [artifact_fact(label, path, kind) for label, path, kind in artifacts],
    }
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return manifest_path


def run_command(command: list[str], timeout_seconds: int) -> dict[str, Any]:
    try:
        result = subprocess.run(
            [sys.executable, *command],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout_seconds,
        )
    except subprocess.TimeoutExpired as exc:
        return {
            "command": [sys.executable, *command],
            "returncode": None,
            "stdout": (exc.stdout or "").strip() if isinstance(exc.stdout, str) else "",
            "stderr": (exc.stderr or "").strip() if isinstance(exc.stderr, str) else "",
            "status": "fail",
            "failure_code": "command_timeout",
            "timeout_seconds": timeout_seconds,
        }
    return {
        "command": [sys.executable, *command],
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "status": "pass" if result.returncode == 0 else "fail",
        "timeout_seconds": timeout_seconds,
    }


def render_quota_check(args: argparse.Namespace) -> None:
    if args.width < 1 or args.width > MAX_RENDER_WIDTH:
        raise AdapterError("quota_exceeded", "Render width is outside the adapter quota.", {"max_width": MAX_RENDER_WIDTH})
    if args.height < 1 or args.height > MAX_RENDER_HEIGHT:
        raise AdapterError("quota_exceeded", "Render height is outside the adapter quota.", {"max_height": MAX_RENDER_HEIGHT})
    if args.frames < 1 or args.frames > MAX_RENDER_FRAMES:
        raise AdapterError("quota_exceeded", "Render frame count is outside the adapter quota.", {"max_frames": MAX_RENDER_FRAMES})
    if args.delta_ms < 1 or args.delta_ms > MAX_RENDER_DELTA_MS:
        raise AdapterError("quota_exceeded", "Render delta is outside the adapter quota.", {"max_delta_ms": MAX_RENDER_DELTA_MS})


def command_status(_args: argparse.Namespace) -> int:
    state = load_toml(STATE_PATH)
    version = load_toml(VERSION_PATH)
    payload = {
        "state_id": state["state_id"],
        "as_of": state["as_of"],
        "public_release": state["authority"]["public_release"],
        "development_head": state["authority"]["development_head"],
        "active_program": state["authority"]["active_program"],
        "compatibility_policy": state["compatibility"]["policy"],
        "proof_kernel": state.get("proof_kernel", {}),
        "project_adapter": {
            "contract": state.get("project_adapter", {}).get("contract"),
            "capability_bindings": state.get("project_adapter", {}).get("capability_bindings"),
            "artifact_profile_audit_roots": state.get("project_adapter", {}).get("artifact_profile_audit_roots"),
            "output_root": state.get("project_adapter", {}).get("output_root"),
            "receipt_schemas": repo_path(RECEIPT_SCHEMAS),
        },
        "version": {
            "runtime_version": version["release"]["runtime_version"],
            "schema_version": version["schema_version"],
        },
    }
    write_json(envelope("status", "informational", payload))
    return 0


def command_capabilities(_args: argparse.Namespace) -> int:
    bindings = load_json(CAPABILITY_BINDINGS)
    payload = {
        "binding_ref": repo_path(CAPABILITY_BINDINGS),
        "binding_sha256": sha256_file(CAPABILITY_BINDINGS),
        "receipt_schema_ref": repo_path(RECEIPT_SCHEMAS),
        "receipt_schema_sha256": sha256_file(RECEIPT_SCHEMAS),
        "artifact_profile_audit_roots_ref": repo_path(ARTIFACT_PROFILE_AUDIT_ROOTS),
        "artifact_profile_audit_roots_sha256": sha256_file(ARTIFACT_PROFILE_AUDIT_ROOTS),
        "output_root": repo_path(INVOCATION_OUTPUT_ROOT),
        "capabilities": bindings.get("capabilities", []),
        "refusals": [
            "source mutation",
            "branch merge",
            "release publication",
            "compatibility certification",
            "visual artistic acceptance",
        ],
    }
    write_json(envelope("capabilities", "informational", payload))
    return 0


def command_catalog(args: argparse.Namespace) -> int:
    inventory = load_json(GENERATED_INVENTORY)
    products = inventory.get("products", [])
    payload: dict[str, Any] = {
        "inventory_ref": repo_path(GENERATED_INVENTORY),
        "schema_version": inventory.get("schema_version"),
        "generated_by": inventory.get("generated_by"),
        "product_count": inventory.get("product_count"),
        "saver_count": inventory.get("saver_count"),
        "artifact_profile_count": inventory.get("artifact_profile_count"),
        "products": [
            {
                "key": product.get("key"),
                "kind": product.get("kind"),
                "release_channel": product.get("release_channel"),
                "maturity": product.get("maturity"),
            }
            for product in products
        ],
    }
    if args.full:
        payload["inventory"] = inventory
    write_json(envelope("catalog", "informational", payload))
    return 0


def command_validate(_args: argparse.Namespace) -> int:
    runs = [run_command(command, timeout_seconds=180) for command in CORE_VALIDATION]
    status = "pass" if all(run["returncode"] == 0 for run in runs) else "fail"
    payload = {
        "validation_class": "bounded-core",
        "command_binding": command_binding_payload("validate", ["validate"], []),
        "runs": runs,
    }
    write_json(envelope("validate", status, payload))
    return exit_code_for_status(status)


def command_build(args: argparse.Namespace) -> int:
    try:
        output_dir = invocation_dir("build", args.invocation_id)
    except AdapterError as exc:
        return blocked("build", exc)
    before_source = source_payload()
    command = [
        str(BUILDCTL.relative_to(ROOT)),
        "build",
        "--profile",
        args.profile,
        "--output-dir",
        str(output_dir),
    ]
    if args.dry_run:
        command.append("--dry-run")
    build_run = run_command(command, timeout_seconds=1200)
    receipt_path = output_dir / "build-receipt.json"
    receipt = load_json(receipt_path) if receipt_path.exists() else {}
    receipt_status = str(receipt.get("status", "fail"))
    manifest_path = write_artifact_manifest(
        output_dir,
        "build",
        [
            ("build_receipt", receipt_path, "build-receipt-json"),
            ("build_log", output_dir / "build.log", "build-log"),
        ],
    )
    if receipt_status == "blocked":
        status = "blocked"
    elif build_run["returncode"] != 0 or receipt_status == "fail":
        status = "fail"
    elif receipt_status == "pass":
        status = "pass"
    else:
        status = "informational"
    payload = {
        "capability": BUILD_CAPABILITIES[args.profile],
        "invocation_id": args.invocation_id,
        "profile": args.profile,
        "dry_run": bool(args.dry_run),
        "output_dir": repo_path(output_dir),
        "artifact_manifest_ref": repo_path(manifest_path),
        "before_source": before_source,
        "after_source": source_payload(),
        "command_binding": command_binding_payload(
            "build",
            build_run["command"],
            [BUILDCTL, ARTIFACT_PROFILE_AUDIT_ROOTS, PE_AUDIT],
            BUILD_CAPABILITIES[args.profile],
        ),
        "run": build_run,
        "build_receipt_ref": repo_path(receipt_path),
        "build_receipt_status": receipt_status,
        "build_lane": receipt.get("build_lane"),
        "artifact_sets": receipt.get("artifact_sets", []),
        "manifest_results": receipt.get("manifest_results", []),
        "audit_results": receipt.get("audit_results", []),
        "claim_boundary": receipt.get(
            "claim_boundary",
            "build, artifact-set, and binary-audit facts only; not runtime compatibility certification or release promotion",
        ),
    }
    write_json(envelope("build", status, payload))
    return exit_code_for_status(status)


def command_render(args: argparse.Namespace) -> int:
    try:
        render_quota_check(args)
        output_dir = invocation_dir("render", args.invocation_id)
    except AdapterError as exc:
        return blocked("render", exc)
    before_source = source_payload()
    command = [
        str(SSLAB.relative_to(ROOT)),
        "render",
        "--product",
        args.product,
        "--preset",
        args.preset,
        "--width",
        str(args.width),
        "--height",
        str(args.height),
        "--seed",
        str(args.seed),
        "--frames",
        str(args.frames),
        "--delta-ms",
        str(args.delta_ms),
        "--output-dir",
        str(output_dir),
    ]
    render_run = run_command(
        command,
        timeout_seconds=30,
    )
    proof_path = output_dir / "proof.json"
    capture_path = output_dir / "capture.ppm"
    hash_path = output_dir / "capture.sha256"
    proof = load_json(proof_path) if proof_path.exists() else {}
    manifest_path = write_artifact_manifest(
        output_dir,
        "render",
        [
            ("capture", capture_path, "ppm-p3-capture"),
            ("proof", proof_path, "proof-json"),
            ("capture_hash", hash_path, "sha256-text"),
        ],
    )
    status = "pass" if render_run["returncode"] == 0 else "fail"
    payload = {
        "capability": "screensave.proof.nocturne.render",
        "render_kind": "proof-kernel-v0-nocturne",
        "invocation_id": args.invocation_id,
        "output_dir": repo_path(output_dir),
        "artifact_manifest_ref": repo_path(manifest_path),
        "before_source": before_source,
        "after_source": source_payload(),
        "command_binding": command_binding_payload("render", render_run["command"], [SSLAB]),
        "run": render_run,
        "proof_ref": repo_path(proof_path),
        "capture_ref": repo_path(capture_path),
        "capture_sha256": proof.get("capture", {}).get("sha256"),
    }
    write_json(envelope("render", status, payload))
    return exit_code_for_status(status)


def command_compare(args: argparse.Namespace) -> int:
    try:
        output_dir = invocation_dir("compare", args.invocation_id)
        actual_path = resolve_input_file("actual", args.actual, APPROVED_COMPARE_INPUT_ROOTS)
        expected_path = resolve_input_file("expected", args.expected, APPROVED_COMPARE_INPUT_ROOTS)
    except AdapterError as exc:
        return blocked("compare", exc)
    before_source = source_payload()
    output_json = output_dir / "comparison.json"
    command = [
        str(SSLAB.relative_to(ROOT)),
        "compare",
        "--actual",
        str(actual_path),
        "--expected",
        str(expected_path),
        "--class",
        args.comparison_class,
        "--tolerance",
        str(args.tolerance),
        "--mean-tolerance",
        str(args.mean_tolerance),
        "--output-json",
        str(output_json),
    ]
    compare_run = run_command(
        command,
        timeout_seconds=30,
    )
    comparison = load_json(output_json) if output_json.exists() else {}
    manifest_path = write_artifact_manifest(
        output_dir,
        "compare",
        [
            ("comparison", output_json, "comparison-json"),
        ],
    )
    status = "pass" if compare_run["returncode"] == 0 else "fail"
    payload = {
        "capability": "screensave.proof.capture.compare",
        "invocation_id": args.invocation_id,
        "output_dir": repo_path(output_dir),
        "comparison_ref": repo_path(output_json),
        "artifact_manifest_ref": repo_path(manifest_path),
        "before_source": before_source,
        "after_source": source_payload(),
        "command_binding": command_binding_payload("compare", compare_run["command"], [SSLAB]),
        "inputs": {
            "actual": artifact_fact("actual", actual_path, "ppm-p3-capture"),
            "expected": artifact_fact("expected", expected_path, "ppm-p3-capture"),
        },
        "run": compare_run,
        "comparison_status": comparison.get("status"),
        "comparison_class": comparison.get("class"),
        "comparison_metrics": comparison.get("metrics", {}),
    }
    write_json(envelope("compare", status, payload))
    return exit_code_for_status(status)


def command_audit(args: argparse.Namespace) -> int:
    try:
        output_dir = invocation_dir("audit", args.invocation_id)
        artifact_profiles, audit_paths = resolve_audit_profiles(args.artifact_profile)
    except AdapterError as exc:
        return blocked("audit", exc)
    before_source = source_payload()
    output_path = output_dir / "pe-audit.txt"
    json_path = output_dir / "pe-audit.json"
    command = [str(PE_AUDIT.relative_to(ROOT)), "--output", str(output_path), "--json-output", str(json_path)]
    if args.fail_on_violation:
        command.append("--fail-on-violation")
    command.extend(str(path) for path in audit_paths)
    audit_run = run_command(command, timeout_seconds=60)
    report = output_path.read_text(encoding="utf-8") if output_path.exists() else ""
    audit_result = load_json(json_path) if json_path.exists() else {}
    violation_count = int(audit_result.get("violation_count", report.count("VIOLATION:")))
    artifact_count = int(audit_result.get("artifact_count", 0))
    audit_status = str(audit_result.get("status", "fail"))
    manifest_path = write_artifact_manifest(
        output_dir,
        "audit",
        [
            ("pe_audit", output_path, "text-report"),
            ("pe_audit_json", json_path, "pe-audit-json"),
        ],
    )
    if audit_status == "blocked" or artifact_count <= 0:
        status = "blocked"
    elif audit_run["returncode"] != 0 or audit_status == "fail":
        status = "fail"
    elif args.fail_on_violation:
        status = "pass"
    else:
        status = "informational"
    payload = {
        "capability": "screensave.artifact.pe.audit",
        "invocation_id": args.invocation_id,
        "output_dir": repo_path(output_dir),
        "audit_ref": repo_path(output_path),
        "audit_json_ref": repo_path(json_path),
        "artifact_manifest_ref": repo_path(manifest_path),
        "before_source": before_source,
        "after_source": source_payload(),
        "command_binding": command_binding_payload("audit", audit_run["command"], [PE_AUDIT, ARTIFACT_PROFILE_AUDIT_ROOTS]),
        "artifact_profiles": artifact_profiles,
        "artifact_profile_audit_roots_ref": repo_path(ARTIFACT_PROFILE_AUDIT_ROOTS),
        "input_paths": [repo_path(path) for path in audit_paths],
        "run": audit_run,
        "audit_status": audit_status,
        "artifact_count": artifact_count,
        "missing_inputs": audit_result.get("missing_inputs", []),
        "parse_error_count": int(audit_result.get("parse_error_count", 0)),
        "violation_count": violation_count,
        "fail_on_violation": bool(args.fail_on_violation),
        "claim_boundary": "binary facts only; not compatibility certification",
    }
    write_json(envelope("audit", status, payload))
    return exit_code_for_status(status)


def command_proof(args: argparse.Namespace) -> int:
    try:
        output_dir = invocation_dir("proof", args.invocation_id)
    except AdapterError as exc:
        return blocked("proof", exc)
    before_source = source_payload()
    comparison_path = output_dir / "comparison.json"
    receipt_path = output_dir / "adapter-proof.json"
    pe_audit_path = output_dir / "pe-audit.txt"
    pe_audit_json_path = output_dir / "pe-audit.json"

    render_command = [
        str(SSLAB.relative_to(ROOT)),
        "render",
        "--product",
        "nocturne",
        "--preset",
        "observatory_night",
        "--width",
        "96",
        "--height",
        "54",
        "--seed",
        "1536",
        "--frames",
        "8",
        "--delta-ms",
        "100",
        "--output-dir",
        str(output_dir),
    ]
    compare_command = [
        str(SSLAB.relative_to(ROOT)),
        "compare",
        "--actual",
        str(output_dir / "capture.ppm"),
        "--expected",
        str(COMMITTED_CANARY),
        "--class",
        "exact",
        "--output-json",
        str(comparison_path),
    ]
    audit_profiles, audit_paths = resolve_audit_profiles(["windows_current_x86_scr"])
    audit_command = [
        str(PE_AUDIT.relative_to(ROOT)),
        "--output",
        str(pe_audit_path),
        "--json-output",
        str(pe_audit_json_path),
        "--fail-on-violation",
    ]
    audit_command.extend(str(path) for path in audit_paths)
    render_run = run_command(render_command, timeout_seconds=30)
    compare_run = run_command(compare_command, timeout_seconds=30)
    audit_run = run_command(audit_command, timeout_seconds=60)

    comparison = {}
    proof = {}
    if comparison_path.exists():
        comparison = json.loads(comparison_path.read_text(encoding="utf-8"))
    proof_path = output_dir / "proof.json"
    if proof_path.exists():
        proof = json.loads(proof_path.read_text(encoding="utf-8"))
    pe_audit_report = pe_audit_path.read_text(encoding="utf-8") if pe_audit_path.exists() else ""
    pe_audit_json = load_json(pe_audit_json_path) if pe_audit_json_path.exists() else {}
    pe_audit_status = str(pe_audit_json.get("status", "fail"))
    pe_audit_artifact_count = int(pe_audit_json.get("artifact_count", 0))
    pe_audit_violation_count = int(pe_audit_json.get("violation_count", pe_audit_report.count("VIOLATION:")))

    if render_run["returncode"] != 0 or compare_run["returncode"] != 0:
        status = "fail"
    elif pe_audit_status == "blocked" or pe_audit_artifact_count <= 0:
        status = "blocked"
    elif audit_run["returncode"] != 0 or pe_audit_status == "fail":
        status = "fail"
    else:
        status = "pass"
    manifest_path = output_dir / "artifact-manifest.json"
    payload = {
        "capability": "screensave.proof.nocturne.exact",
        "proof_kind": "proof-kernel-v0-nocturne-exact",
        "invocation_id": args.invocation_id,
        "output_dir": repo_path(output_dir),
        "artifact_manifest_ref": repo_path(manifest_path),
        "before_source": before_source,
        "after_source": source_payload(),
        "command_binding": command_binding_payload(
            "proof",
            render_run["command"] + ["&&"] + compare_run["command"] + ["&&"] + audit_run["command"],
            [SSLAB, PE_AUDIT, ARTIFACT_PROFILE_AUDIT_ROOTS],
        ),
        "render": render_run,
        "compare": compare_run,
        "pe_audit": audit_run,
        "pe_audit_ref": repo_path(pe_audit_path),
        "pe_audit_json_ref": repo_path(pe_audit_json_path),
        "pe_audit_status": pe_audit_status,
        "pe_audit_artifact_count": pe_audit_artifact_count,
        "pe_audit_missing_inputs": pe_audit_json.get("missing_inputs", []),
        "pe_audit_parse_error_count": int(pe_audit_json.get("parse_error_count", 0)),
        "pe_audit_violation_count": pe_audit_violation_count,
        "pe_audit_artifact_profiles": audit_profiles,
        "pe_audit_claim_boundary": "binary facts only; not compatibility certification",
        "artifact_profile_audit_roots_ref": repo_path(ARTIFACT_PROFILE_AUDIT_ROOTS),
        "proof_ref": repo_path(proof_path),
        "comparison_ref": repo_path(comparison_path),
        "capture_ref": repo_path(output_dir / "capture.ppm"),
        "capture_sha256": proof.get("capture", {}).get("sha256"),
        "comparison_status": comparison.get("status"),
        "comparison_metrics": comparison.get("metrics", {}),
    }
    receipt = envelope("proof", status, payload)
    receipt_path.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    write_artifact_manifest(
        output_dir,
        "proof",
        [
            ("capture", output_dir / "capture.ppm", "ppm-p3-capture"),
            ("proof", proof_path, "proof-json"),
            ("comparison", comparison_path, "comparison-json"),
            ("pe_audit", pe_audit_path, "text-report"),
            ("pe_audit_json", pe_audit_json_path, "pe-audit-json"),
            ("adapter_receipt", receipt_path, "adapter-receipt-json"),
        ],
    )
    write_json(receipt)
    return exit_code_for_status(status)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    status = subparsers.add_parser("status", help="Report ScreenSave project status.")
    status.set_defaults(func=command_status)

    capabilities = subparsers.add_parser("capabilities", help="Report adapter capabilities.")
    capabilities.set_defaults(func=command_capabilities)

    catalog = subparsers.add_parser("catalog", help="Report generated product catalog inventory.")
    catalog.add_argument("--full", action="store_true", help="Include the full generated inventory payload.")
    catalog.set_defaults(func=command_catalog)

    validate = subparsers.add_parser("validate", help="Run bounded ScreenSave validation.")
    validate.set_defaults(func=command_validate)

    build = subparsers.add_parser("build", help="Run a fixed ScreenSave build profile.")
    build.add_argument("--invocation-id", default="local", help="Safe id for the contained adapter output root.")
    build.add_argument("--profile", required=True, choices=sorted(BUILD_CAPABILITIES))
    build.add_argument("--dry-run", action="store_true", help="Record a fixed build command plan without invoking MSBuild.")
    build.set_defaults(func=command_build)

    render = subparsers.add_parser("render", help="Render the Proof Kernel v0 Nocturne canary.")
    render.add_argument("--invocation-id", default="local", help="Safe id for the contained adapter output root.")
    render.add_argument("--product", default="nocturne", choices=["nocturne"])
    render.add_argument("--preset", default="observatory_night")
    render.add_argument("--width", type=int, default=96)
    render.add_argument("--height", type=int, default=54)
    render.add_argument("--seed", type=int, default=1536)
    render.add_argument("--frames", type=int, default=8)
    render.add_argument("--delta-ms", type=int, default=100)
    render.set_defaults(func=command_render)

    compare = subparsers.add_parser("compare", help="Compare two proof-kernel captures.")
    compare.add_argument("--invocation-id", default="local", help="Safe id for the contained adapter output root.")
    compare.add_argument("--actual", default=str(COMMITTED_CANARY.relative_to(ROOT)))
    compare.add_argument("--expected", default=str(COMMITTED_CANARY.relative_to(ROOT)))
    compare.add_argument(
        "--class",
        dest="comparison_class",
        default="exact",
        choices=["exact", "tolerant", "perceptual", "observational"],
    )
    compare.add_argument("--tolerance", type=int, default=0)
    compare.add_argument("--mean-tolerance", type=float, default=0.0)
    compare.set_defaults(func=command_compare)

    audit = subparsers.add_parser("audit", help="Run the ScreenSave PE artifact audit.")
    audit.add_argument("--invocation-id", default="local", help="Safe id for the contained adapter output root.")
    audit.add_argument(
        "--artifact-profile",
        action="append",
        help="Artifact profile key to audit. Defaults to all PE artifact profiles admitted by the adapter.",
    )
    audit.add_argument("--fail-on-violation", action="store_true")
    audit.set_defaults(func=command_audit)

    proof = subparsers.add_parser("proof", help="Run Proof Kernel v0 Nocturne proof.")
    proof.add_argument("--invocation-id", default="local", help="Safe id for the contained adapter output root.")
    proof.set_defaults(func=command_proof)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
