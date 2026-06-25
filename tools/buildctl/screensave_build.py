"""Run fixed ScreenSave build profiles and produce build receipts."""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
ARTIFACT_MANIFEST = ROOT / "tools" / "artifactmanifest" / "artifactmanifest.py"
PE_AUDIT = ROOT / "tools" / "scripts" / "audit_pe_artifacts.py"
OUTPUT_ROOT = ROOT / "out" / "buildctl"

PROFILES: dict[str, dict[str, Any]] = {
    "windows-current-x86": {
        "description": "Current Windows x86 saver build lane.",
        "build_lane": "msvc-vs2022-release-win32",
        "solution": "build/msvc/vs2022/ScreenSave.sln",
        "configuration": "Release",
        "platform": "Win32",
        "artifact_sets": ["windows_current_x86_scr"],
        "timeout_seconds": 900,
    },
    "windows-current-tools": {
        "description": "Current Windows x86 companion-tool build lane.",
        "build_lane": "msvc-vs2022-release-win32",
        "solution": "build/msvc/vs2022/ScreenSave.sln",
        "configuration": "Release",
        "platform": "Win32",
        "artifact_sets": ["windows_current_tools"],
        "timeout_seconds": 900,
    },
}


def repo_path(path: pathlib.Path) -> str:
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


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


def resolve_msbuild() -> str:
    from_path = shutil.which("msbuild")
    if from_path:
        return from_path

    program_files_x86 = os.environ.get("ProgramFiles(x86)")
    if program_files_x86:
        vswhere = pathlib.Path(program_files_x86) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
        if vswhere.exists():
            try:
                output = subprocess.check_output(
                    [
                        str(vswhere),
                        "-latest",
                        "-requires",
                        "Microsoft.Component.MSBuild",
                        "-find",
                        "MSBuild\\**\\Bin\\MSBuild.exe",
                    ],
                    cwd=ROOT,
                    text=True,
                    stderr=subprocess.DEVNULL,
                ).strip()
            except Exception:
                output = ""
            if output:
                first = output.splitlines()[0].strip()
                if first:
                    return first

    return "msbuild"


def profile_command(profile: dict[str, Any], resolve_tool: bool = False) -> list[str]:
    msbuild = resolve_msbuild() if resolve_tool else "msbuild"
    return [
        msbuild,
        str(profile["solution"]),
        "/m",
        f"/p:Configuration={profile['configuration']}",
        f"/p:Platform={profile['platform']}",
    ]


def run_process(command: list[str], timeout_seconds: int, output_log: pathlib.Path | None = None) -> dict[str, Any]:
    try:
        result = subprocess.run(
            command,
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout_seconds,
        )
    except FileNotFoundError as exc:
        return {
            "command": command,
            "returncode": None,
            "status": "blocked",
            "failure_code": "command_not_found",
            "stdout": "",
            "stderr": str(exc),
            "timeout_seconds": timeout_seconds,
        }
    except subprocess.TimeoutExpired as exc:
        return {
            "command": command,
            "returncode": None,
            "status": "fail",
            "failure_code": "command_timeout",
            "stdout": (exc.stdout or "").strip() if isinstance(exc.stdout, str) else "",
            "stderr": (exc.stderr or "").strip() if isinstance(exc.stderr, str) else "",
            "timeout_seconds": timeout_seconds,
        }

    stdout = result.stdout.strip()
    stderr = result.stderr.strip()
    if output_log is not None:
        output_log.parent.mkdir(parents=True, exist_ok=True)
        output_log.write_text(stdout + ("\n\n[stderr]\n" + stderr if stderr else "") + "\n", encoding="utf-8")

    return {
        "command": command,
        "returncode": result.returncode,
        "status": "pass" if result.returncode == 0 else "fail",
        "stdout": stdout[-4000:],
        "stderr": stderr[-4000:],
        "timeout_seconds": timeout_seconds,
    }


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def manifest_command(artifact_set: str, output_path: pathlib.Path) -> list[str]:
    return [
        sys.executable,
        str(ARTIFACT_MANIFEST),
        "generate",
        "--artifact-set",
        artifact_set,
        "--output",
        str(output_path),
    ]


def audit_command(manifest_path: pathlib.Path, profile_key: str, output_path: pathlib.Path, json_path: pathlib.Path) -> list[str]:
    return [
        sys.executable,
        str(PE_AUDIT),
        "--artifact-manifest",
        str(manifest_path),
        "--artifact-profile",
        profile_key,
        "--output",
        str(output_path),
        "--json-output",
        str(json_path),
    ]


def status_from_runs(build_run: dict[str, Any] | None, manifests: list[dict[str, Any]], audits: list[dict[str, Any]]) -> str:
    if build_run is None:
        return "informational"
    if build_run.get("status") == "blocked":
        return "blocked"
    if build_run.get("returncode") != 0:
        return "fail"
    if any(item.get("run", {}).get("returncode") != 0 for item in manifests):
        return "fail"
    audit_statuses = [str(item.get("audit", {}).get("status", "fail")) for item in audits]
    if any(status == "blocked" for status in audit_statuses):
        return "blocked"
    if any(item.get("run", {}).get("returncode") != 0 or status == "fail" for item, status in zip(audits, audit_statuses)):
        return "fail"
    return "pass"


def command_build(args: argparse.Namespace) -> int:
    profile = PROFILES[args.profile]
    output_dir = (ROOT / args.output_dir).resolve() if args.output_dir else (OUTPUT_ROOT / args.profile).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    build_command = profile_command(profile, resolve_tool=not args.dry_run)
    build_run = None
    if not args.dry_run:
        build_run = run_process(build_command, int(profile["timeout_seconds"]), output_dir / "build.log")

    manifest_results: list[dict[str, Any]] = []
    audit_results: list[dict[str, Any]] = []
    if build_run is not None and build_run.get("returncode") == 0:
        for artifact_set in profile["artifact_sets"]:
            manifest_path = output_dir / f"{artifact_set}.manifest.json"
            manifest_run = run_process(manifest_command(artifact_set, manifest_path), timeout_seconds=120)
            manifest_payload = load_json(manifest_path) if manifest_path.exists() else {}
            manifest_results.append(
                {
                    "artifact_set": artifact_set,
                    "manifest_ref": repo_path(manifest_path),
                    "manifest_status": manifest_payload.get("status"),
                    "manifest": manifest_payload,
                    "run": manifest_run,
                }
            )
            profile_key = manifest_payload.get("artifact_set", {}).get("artifact_profile")
            if profile_key:
                audit_path = output_dir / f"{artifact_set}.pe-audit.txt"
                audit_json_path = output_dir / f"{artifact_set}.pe-audit.json"
                audit_run = run_process(audit_command(manifest_path, str(profile_key), audit_path, audit_json_path), timeout_seconds=120)
                audit_payload = load_json(audit_json_path) if audit_json_path.exists() else {}
                audit_results.append(
                    {
                        "artifact_set": artifact_set,
                        "artifact_profile": profile_key,
                        "audit_ref": repo_path(audit_path),
                        "audit_json_ref": repo_path(audit_json_path),
                        "audit": audit_payload,
                        "run": audit_run,
                    }
                )

    status = status_from_runs(build_run, manifest_results, audit_results)
    receipt = {
        "build_receipt_schema": "screensave-build-receipt-v0",
        "profile": args.profile,
        "status": status,
        "dry_run": bool(args.dry_run),
        "source": source_payload(),
        "output_dir": repo_path(output_dir),
        "description": profile["description"],
        "build_lane": profile["build_lane"],
        "build_command": build_command,
        "build_run": build_run,
        "artifact_sets": list(profile["artifact_sets"]),
        "manifest_results": manifest_results,
        "audit_results": audit_results,
        "claim_boundary": "build, artifact-set, and binary-audit facts only; not runtime compatibility certification or release promotion",
        "limits": [
            "Only named build profiles are accepted.",
            "No arbitrary compiler, linker, or MSBuild arguments are exposed.",
            "Dry-run receipts are command plans, not build evidence.",
        ],
    }
    receipt_path = output_dir / "build-receipt.json"
    receipt_path.write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps(receipt, indent=2, sort_keys=True))

    if status in {"pass", "informational"}:
        return 0
    if status == "blocked":
        return 2
    return 1


def command_profiles(_args: argparse.Namespace) -> int:
    payload = {
        "build_profiles_schema": "screensave-build-profiles-v0",
        "profiles": [
            {
                "key": key,
                "description": value["description"],
                "build_lane": value["build_lane"],
                "configuration": value["configuration"],
                "platform": value["platform"],
                "solution": value["solution"],
                "artifact_sets": value["artifact_sets"],
                "command": profile_command(value),
            }
            for key, value in sorted(PROFILES.items())
        ],
        "policy": "fixed profiles only; no arbitrary build arguments",
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    profiles = subparsers.add_parser("profiles", help="List fixed build profiles.")
    profiles.set_defaults(func=command_profiles)

    build = subparsers.add_parser("build", help="Run a fixed build profile.")
    build.add_argument("--profile", required=True, choices=sorted(PROFILES))
    build.add_argument("--output-dir", help="Optional output directory for receipts and logs.")
    build.add_argument("--dry-run", action="store_true", help="Write a command-plan receipt without invoking MSBuild.")
    build.set_defaults(func=command_build)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
