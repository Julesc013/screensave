"""Validate the ScreenSave project adapter command surface."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
ADAPTER = ROOT / "tools" / "project_adapter" / "screensave_project.py"

REQUIRED_PATHS = [
    ROOT / "contracts" / "project_adapter_v0.md",
    ROOT / "tools" / "project_adapter" / "README.md",
    ADAPTER,
]

REQUIRED_TEXT = {
    ADAPTER: [
        "screensave-project-adapter-v0",
        "def command_status",
        "def command_capabilities",
        "def command_catalog",
        "def command_validate",
        "def command_render",
        "def command_compare",
        "def command_audit",
        "def command_proof",
    ],
    ROOT / "contracts" / "project_adapter_v0.md": [
        "status",
        "capabilities",
        "catalog",
        "validate",
        "render",
        "compare",
        "audit",
        "proof",
        "AIDE may consume receipts",
    ],
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_adapter(args: list[str]) -> dict:
    output = subprocess.check_output([sys.executable, str(ADAPTER), *args], cwd=ROOT, text=True)
    return json.loads(output)


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
        require(status.get("payload", {}).get("active_program") == "proof-kernel-v0", "status must report proof-kernel-v0.", errors)

        capabilities = run_adapter(["capabilities"])
        names = {item.get("name") for item in capabilities.get("payload", {}).get("capabilities", [])}
        require("project.catalog" in names, "capabilities must include project.catalog.", errors)
        require("project.validate" in names, "capabilities must include project.validate.", errors)
        require("project.render.nocturne" in names, "capabilities must include project.render.nocturne.", errors)
        require("project.compare.capture" in names, "capabilities must include project.compare.capture.", errors)
        require("project.audit.pe" in names, "capabilities must include project.audit.pe.", errors)
        require("project.proof.nocturne" in names, "capabilities must include project.proof.nocturne.", errors)

        catalog = run_adapter(["catalog"])
        require(catalog.get("payload", {}).get("saver_count", 0) >= 19, "adapter catalog must report current saver family.", errors)

        validation = run_adapter(["validate"])
        require(validation.get("status") == "pass", "adapter validate must pass.", errors)

        with tempfile.TemporaryDirectory() as temp_dir:
            render_dir = pathlib.Path(temp_dir) / "render"
            render = run_adapter(["render", "--output-dir", str(render_dir)])
            require(render.get("status") == "pass", "adapter render must pass.", errors)
            capture_path = render_dir / "capture.ppm"
            require(capture_path.exists(), "adapter render must write capture.ppm.", errors)

            compare_path = pathlib.Path(temp_dir) / "comparison.json"
            compare = run_adapter(["compare", "--actual", str(capture_path), "--output-json", str(compare_path)])
            require(compare.get("status") == "pass", "adapter compare must pass.", errors)
            require(compare.get("payload", {}).get("comparison_status") == "pass", "adapter compare comparison_status must pass.", errors)

            audit_path = pathlib.Path(temp_dir) / "pe-audit.txt"
            audit = run_adapter(["audit", "--output", str(audit_path)])
            require(audit.get("status") == "informational", "adapter audit must report informational facts by default.", errors)
            require(audit_path.exists(), "adapter audit must write a report.", errors)

            proof_dir = pathlib.Path(temp_dir) / "proof"
            proof = run_adapter(["proof", "--output-dir", str(proof_dir)])
            require(proof.get("status") == "pass", "adapter proof must pass.", errors)
            require(proof.get("payload", {}).get("comparison_status") == "pass", "adapter proof comparison must pass.", errors)
            require((proof_dir / "adapter-proof.json").exists(), "adapter proof must write adapter-proof.json.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Project adapter checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
