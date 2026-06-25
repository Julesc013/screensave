"""Validate ScreenSave Proof Kernel v0 scaffolding and deterministic canary output."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
SSLAB = ROOT / "tools" / "sslab" / "sslab.py"
EVIDENCE_DIR = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne"

REQUIRED_PATHS = [
    ROOT / "contracts" / "proof_kernel_v0.md",
    ROOT / "contracts" / "surface_rgba8_v0.md",
    ROOT / "platform" / "include" / "screensave" / "private" / "surface_rgba8.h",
    ROOT / "platform" / "include" / "screensave" / "private" / "soft_renderer.h",
    ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c",
    ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c",
    ROOT / "tools" / "sslab" / "README.md",
    SSLAB,
    ROOT / "tools" / "sslab" / "nocturne_canary_runner.c",
    ROOT / "tools" / "scripts" / "check_compiled_nocturne_runner.py",
    EVIDENCE_DIR / "README.md",
    EVIDENCE_DIR / "capture.ppm",
    EVIDENCE_DIR / "capture.sha256",
    EVIDENCE_DIR / "proof.json",
]

REQUIRED_TEXT = {
    ROOT / "platform" / "include" / "screensave" / "private" / "surface_rgba8.h": [
        "screensave_rgba8_surface",
        "screensave_rgba8_surface_init",
        "screensave_rgba8_surface_checksum",
    ],
    ROOT / "platform" / "include" / "screensave" / "private" / "soft_renderer.h": [
        "screensave_soft_fill_rect",
        "screensave_soft_draw_frame_rect",
        "screensave_soft_draw_line",
        "screensave_soft_draw_polyline",
    ],
    ROOT / "tools" / "sslab" / "sslab.py": [
        "proof-bundle-v0",
        "sslab-comparison-v0",
        "proof-kernel-v0",
        "surface = Surface",
        "def render_nocturne",
        "def compare_captures",
    ],
    ROOT / "tools" / "sslab" / "nocturne_canary_runner.c": [
        "compiled-nocturne",
        "product-session",
        "nocturne_create_session",
        "nocturne_step_session",
        "nocturne_render_session",
        "screensave/private/soft_renderer.h",
    ],
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_sslab(output_dir: pathlib.Path) -> dict:
    subprocess.check_call(
        [
            sys.executable,
            str(SSLAB),
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
        ],
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
    )
    return json.loads((output_dir / "proof.json").read_text(encoding="utf-8"))


def validate_determinism(errors: list[str]) -> None:
    with tempfile.TemporaryDirectory() as first, tempfile.TemporaryDirectory() as second:
        first_proof = run_sslab(pathlib.Path(first))
        second_proof = run_sslab(pathlib.Path(second))

        first_hash = first_proof.get("capture", {}).get("sha256")
        second_hash = second_proof.get("capture", {}).get("sha256")
        require(first_hash == second_hash, "Proof Kernel Nocturne capture must be deterministic across repeated runs.", errors)

        committed_hash_path = EVIDENCE_DIR / "capture.sha256"
        if committed_hash_path.exists():
            committed_hash = committed_hash_path.read_text(encoding="ascii").strip()
            require(committed_hash == first_hash, "Committed Nocturne capture hash must match the current deterministic output.", errors)

        require(first_proof.get("proof_schema") == "proof-bundle-v0", "sslab proof must use proof-bundle-v0.", errors)
        require(first_proof.get("proof_kernel") == "proof-kernel-v0", "sslab proof must identify proof-kernel-v0.", errors)
        require(first_proof.get("runtime", {}).get("renderer") == "soft-reference-v0", "sslab proof must record soft-reference-v0.", errors)
        require(first_proof.get("status") == "informational", "sslab proof must remain informational at v0.", errors)

        comparison_path = pathlib.Path(first) / "comparison.json"
        subprocess.check_call(
            [
                sys.executable,
                str(SSLAB),
                "compare",
                "--actual",
                str(pathlib.Path(first) / "capture.ppm"),
                "--expected",
                str(pathlib.Path(second) / "capture.ppm"),
                "--class",
                "exact",
                "--output-json",
                str(comparison_path),
            ],
            cwd=ROOT,
            stdout=subprocess.DEVNULL,
        )
        comparison = json.loads(comparison_path.read_text(encoding="utf-8"))
        require(comparison.get("comparison_schema") == "sslab-comparison-v0", "sslab compare must emit sslab-comparison-v0.", errors)
        require(comparison.get("status") == "pass", "Repeated Nocturne captures must compare exactly.", errors)
        require(comparison.get("metrics", {}).get("changed_pixels") == 0, "Exact repeated Nocturne comparison must have zero changed pixels.", errors)


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing Proof Kernel path: {path.relative_to(ROOT)}", errors)

    for path, needles in REQUIRED_TEXT.items():
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        for needle in needles:
            require(needle in text, f"{path.relative_to(ROOT)} is missing expected text: {needle!r}", errors)

    if not errors:
        validate_determinism(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Proof Kernel checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
