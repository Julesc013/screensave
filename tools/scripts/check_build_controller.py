"""Validate the fixed ScreenSave build controller surface."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILDCTL = ROOT / "tools" / "buildctl" / "screensave_build.py"
BUILDCTL_README = ROOT / "tools" / "buildctl" / "README.md"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_buildctl(args: list[str]) -> dict:
    output = subprocess.check_output([sys.executable, str(BUILDCTL), *args], cwd=ROOT, text=True)
    return json.loads(output)


def main() -> int:
    errors: list[str] = []

    require(BUILDCTL.exists(), "Missing tools/buildctl/screensave_build.py.", errors)
    require(BUILDCTL_README.exists(), "Missing tools/buildctl/README.md.", errors)

    if BUILDCTL.exists():
        text = BUILDCTL.read_text(encoding="utf-8")
        for needle in [
            "windows-current-x86",
            "windows-current-tools",
            "msbuild",
            "windows_current_x86_scr",
            "windows_current_tools",
            "No arbitrary compiler, linker, or MSBuild arguments are exposed.",
        ]:
            require(needle in text, f"build controller is missing expected text: {needle!r}", errors)

    if BUILDCTL_README.exists():
        readme = BUILDCTL_README.read_text(encoding="utf-8")
        require("fixed ScreenSave build-profile controller" in readme, "buildctl README must describe the fixed build-profile controller.", errors)
        require("--dry-run" in readme, "buildctl README must document dry-run receipts.", errors)

    if not errors:
        profiles = run_buildctl(["profiles"])
        profile_map = {item.get("key"): item for item in profiles.get("profiles", [])}
        require(profiles.get("policy") == "fixed profiles only; no arbitrary build arguments", "buildctl profiles must declare fixed-profile policy.", errors)
        require("windows-current-x86" in profile_map, "buildctl must expose windows-current-x86.", errors)
        require("windows-current-tools" in profile_map, "buildctl must expose windows-current-tools.", errors)
        for key, artifact_set in [
            ("windows-current-x86", "windows_current_x86_scr"),
            ("windows-current-tools", "windows_current_tools"),
        ]:
            profile = profile_map.get(key, {})
            command = profile.get("command", [])
            require(command[:2] == ["msbuild", "build/msvc/vs2022/ScreenSave.sln"], f"{key} must use the fixed VS2022 solution.", errors)
            require("/p:Configuration=Release" in command, f"{key} must build Release.", errors)
            require("/p:Platform=Win32" in command, f"{key} must build Win32.", errors)
            require(profile.get("artifact_sets") == [artifact_set], f"{key} must map to {artifact_set}.", errors)

        dry_run = run_buildctl(
            [
                "build",
                "--profile",
                "windows-current-x86",
                "--dry-run",
                "--output-dir",
                "out/buildctl/check-build-controller",
            ]
        )
        require(dry_run.get("status") == "informational", "dry-run build receipt must be informational.", errors)
        require(dry_run.get("dry_run") is True, "dry-run build receipt must set dry_run true.", errors)
        require(dry_run.get("build_run") is None, "dry-run build receipt must not report a build run.", errors)
        require(
            dry_run.get("claim_boundary")
            == "build, artifact-set, and binary-audit facts only; not runtime compatibility certification or release promotion",
            "build receipt must preserve the claim boundary.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Build controller checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
