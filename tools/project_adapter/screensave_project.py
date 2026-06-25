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
DEFAULT_PROOF_DIR = ROOT / "out" / "proof" / "project-adapter" / "nocturne"
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
                "name": "project.validate",
                "command": "validate",
                "effect": "runs ScreenSave-owned validators",
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


def command_validate(_args: argparse.Namespace) -> int:
    runs = [run_command(command) for command in CORE_VALIDATION]
    status = "pass" if all(run["returncode"] == 0 for run in runs) else "fail"
    payload = {
        "validation_class": "bounded-core",
        "runs": runs,
    }
    write_json(envelope("validate", status, payload))
    return 0 if status == "pass" else 1


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
    return 0 if status == "pass" else 1


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    status = subparsers.add_parser("status", help="Report ScreenSave project status.")
    status.set_defaults(func=command_status)

    capabilities = subparsers.add_parser("capabilities", help="Report adapter capabilities.")
    capabilities.set_defaults(func=command_capabilities)

    validate = subparsers.add_parser("validate", help="Run bounded ScreenSave validation.")
    validate.set_defaults(func=command_validate)

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
