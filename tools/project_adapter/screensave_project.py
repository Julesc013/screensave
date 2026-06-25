"""ScreenSave project-owned adapter for proof-native coordination."""

from __future__ import annotations

import argparse
import json
import pathlib
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
DEFAULT_PROOF_DIR = ROOT / "out" / "proof" / "project-adapter" / "nocturne"
DEFAULT_RENDER_DIR = ROOT / "out" / "proof" / "project-adapter" / "render" / "nocturne"
DEFAULT_COMPARE_JSON = ROOT / "out" / "proof" / "project-adapter" / "compare" / "comparison.json"
DEFAULT_AUDIT_REPORT = ROOT / "out" / "proof" / "project-adapter" / "pe-audit.txt"
COMMITTED_CANARY = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne" / "capture.ppm"

CORE_VALIDATION = [
    ["tools/scripts/check_project_state.py"],
    ["tools/scripts/check_catalog_profiles.py"],
    ["tools/scripts/check_catalog_generated.py"],
    ["tools/scripts/check_contracts.py"],
    ["tools/scripts/check_proof_kernel.py"],
    ["tools/scripts/check_docs_basics.py"],
]


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


def run_command(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(
        [sys.executable, *command],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return {
        "command": [sys.executable, *command],
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "status": "pass" if result.returncode == 0 else "fail",
    }


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
        "version": {
            "runtime_version": version["release"]["runtime_version"],
            "schema_version": version["schema_version"],
        },
    }
    write_json(envelope("status", "informational", payload))
    return 0


def command_capabilities(_args: argparse.Namespace) -> int:
    payload = {
        "capabilities": [
            {
                "name": "project.status",
                "command": "status",
                "effect": "read-only",
                "deterministic": True,
            },
            {
                "name": "project.capabilities",
                "command": "capabilities",
                "effect": "read-only",
                "deterministic": True,
            },
            {
                "name": "project.catalog",
                "command": "catalog",
                "effect": "read-only generated catalog inventory",
                "deterministic": True,
            },
            {
                "name": "project.validate",
                "command": "validate",
                "effect": "runs ScreenSave-owned validators",
                "deterministic": True,
            },
            {
                "name": "project.render.nocturne",
                "command": "render",
                "effect": "writes deterministic Nocturne capture artifacts",
                "deterministic": True,
            },
            {
                "name": "project.compare.capture",
                "command": "compare",
                "effect": "writes capture comparison JSON",
                "deterministic": True,
            },
            {
                "name": "project.audit.pe",
                "command": "audit",
                "effect": "writes PE binary-fact audit report",
                "deterministic": True,
            },
            {
                "name": "project.proof.nocturne",
                "command": "proof",
                "effect": "writes proof artifacts under requested output directory",
                "deterministic": True,
            },
        ],
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
    runs = [run_command(command) for command in CORE_VALIDATION]
    status = "pass" if all(run["returncode"] == 0 for run in runs) else "fail"
    payload = {
        "validation_class": "bounded-core",
        "runs": runs,
    }
    write_json(envelope("validate", status, payload))
    return exit_code_for_status(status)


def command_render(args: argparse.Namespace) -> int:
    output_dir = pathlib.Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    render_run = run_command(
        [
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
    )
    proof_path = output_dir / "proof.json"
    proof = load_json(proof_path) if proof_path.exists() else {}
    status = "pass" if render_run["returncode"] == 0 else "fail"
    payload = {
        "render_kind": "proof-kernel-v0-nocturne",
        "output_dir": repo_path(output_dir),
        "run": render_run,
        "proof_ref": repo_path(proof_path),
        "capture_ref": repo_path(output_dir / "capture.ppm"),
        "capture_sha256": proof.get("capture", {}).get("sha256"),
    }
    write_json(envelope("render", status, payload))
    return exit_code_for_status(status)


def command_compare(args: argparse.Namespace) -> int:
    output_json = pathlib.Path(args.output_json)
    if not output_json.is_absolute():
        output_json = ROOT / output_json
    compare_run = run_command(
        [
            str(SSLAB.relative_to(ROOT)),
            "compare",
            "--actual",
            args.actual,
            "--expected",
            args.expected,
            "--class",
            args.comparison_class,
            "--tolerance",
            str(args.tolerance),
            "--mean-tolerance",
            str(args.mean_tolerance),
            "--output-json",
            str(output_json),
        ]
    )
    comparison = load_json(output_json) if output_json.exists() else {}
    status = "pass" if compare_run["returncode"] == 0 else "fail"
    payload = {
        "comparison_ref": repo_path(output_json),
        "run": compare_run,
        "comparison_status": comparison.get("status"),
        "comparison_class": comparison.get("class"),
        "comparison_metrics": comparison.get("metrics", {}),
    }
    write_json(envelope("compare", status, payload))
    return exit_code_for_status(status)


def command_audit(args: argparse.Namespace) -> int:
    output_path = pathlib.Path(args.output)
    if not output_path.is_absolute():
        output_path = ROOT / output_path
    command = [str(PE_AUDIT.relative_to(ROOT)), "--output", str(output_path)]
    if args.fail_on_violation:
        command.append("--fail-on-violation")
    command.extend(args.paths)
    audit_run = run_command(command)
    report = output_path.read_text(encoding="utf-8") if output_path.exists() else ""
    violation_count = report.count("VIOLATION:")
    if audit_run["returncode"] != 0:
        status = "fail"
    elif args.fail_on_violation:
        status = "pass"
    else:
        status = "informational"
    payload = {
        "audit_ref": repo_path(output_path),
        "run": audit_run,
        "violation_count": violation_count,
        "fail_on_violation": bool(args.fail_on_violation),
        "claim_boundary": "binary facts only; not compatibility certification",
    }
    write_json(envelope("audit", status, payload))
    return exit_code_for_status(status)


def command_proof(args: argparse.Namespace) -> int:
    output_dir = pathlib.Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)
    comparison_path = output_dir / "comparison.json"
    receipt_path = output_dir / "adapter-proof.json"

    render_run = run_command(
        [
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
    )
    compare_run = run_command(
        [
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
    )

    comparison = {}
    proof = {}
    if comparison_path.exists():
        comparison = json.loads(comparison_path.read_text(encoding="utf-8"))
    proof_path = output_dir / "proof.json"
    if proof_path.exists():
        proof = json.loads(proof_path.read_text(encoding="utf-8"))

    status = "pass" if render_run["returncode"] == 0 and compare_run["returncode"] == 0 else "fail"
    payload = {
        "proof_kind": "proof-kernel-v0-nocturne-exact",
        "output_dir": repo_path(output_dir),
        "render": render_run,
        "compare": compare_run,
        "proof_ref": repo_path(proof_path),
        "comparison_ref": repo_path(comparison_path),
        "capture_ref": repo_path(output_dir / "capture.ppm"),
        "capture_sha256": proof.get("capture", {}).get("sha256"),
        "comparison_status": comparison.get("status"),
        "comparison_metrics": comparison.get("metrics", {}),
    }
    receipt = envelope("proof", status, payload)
    receipt_path.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8")
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

    render = subparsers.add_parser("render", help="Render the Proof Kernel v0 Nocturne canary.")
    render.add_argument("--product", default="nocturne", choices=["nocturne"])
    render.add_argument("--preset", default="observatory_night")
    render.add_argument("--width", type=int, default=96)
    render.add_argument("--height", type=int, default=54)
    render.add_argument("--seed", type=int, default=1536)
    render.add_argument("--frames", type=int, default=8)
    render.add_argument("--delta-ms", type=int, default=100)
    render.add_argument("--output-dir", default=str(DEFAULT_RENDER_DIR.relative_to(ROOT)))
    render.set_defaults(func=command_render)

    compare = subparsers.add_parser("compare", help="Compare two proof-kernel captures.")
    compare.add_argument("--actual", required=True)
    compare.add_argument("--expected", default=str(COMMITTED_CANARY.relative_to(ROOT)))
    compare.add_argument(
        "--class",
        dest="comparison_class",
        default="exact",
        choices=["exact", "tolerant", "perceptual", "observational"],
    )
    compare.add_argument("--tolerance", type=int, default=0)
    compare.add_argument("--mean-tolerance", type=float, default=0.0)
    compare.add_argument("--output-json", default=str(DEFAULT_COMPARE_JSON.relative_to(ROOT)))
    compare.set_defaults(func=command_compare)

    audit = subparsers.add_parser("audit", help="Run the ScreenSave PE artifact audit.")
    audit.add_argument("paths", nargs="*", help="Optional PE artifact roots or files.")
    audit.add_argument("--output", default=str(DEFAULT_AUDIT_REPORT.relative_to(ROOT)))
    audit.add_argument("--fail-on-violation", action="store_true")
    audit.set_defaults(func=command_audit)

    proof = subparsers.add_parser("proof", help="Run Proof Kernel v0 Nocturne proof.")
    proof.add_argument("--output-dir", default=str(DEFAULT_PROOF_DIR.relative_to(ROOT)))
    proof.set_defaults(func=command_proof)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
