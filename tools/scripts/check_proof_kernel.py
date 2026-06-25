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
        "screensave-nocturne-canary-lifecycle-v0",
        "proof-kernel-v0",
        "compile_nocturne_runner",
        "compiled-product-session",
        "RUNNER_SOURCES",
        "def render_nocturne",
        "def lifecycle_nocturne",
        "def compare_captures",
    ],
    ROOT / "tools" / "sslab" / "nocturne_canary_runner.c": [
        "compiled-nocturne",
        "product-session",
        "nocturne_create_session",
        "nocturne_resize_session",
        "nocturne_step_session",
        "nocturne_render_session",
        "runner_write_lifecycle_json",
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


def validate_committed_proof(errors: list[str]) -> None:
    proof_path = EVIDENCE_DIR / "proof.json"
    if not proof_path.exists():
        return
    proof = json.loads(proof_path.read_text(encoding="utf-8"))
    implementation_paths = {item.get("path") for item in proof.get("implementation", [])}
    require(proof.get("source", {}).get("dirty") is False, "Committed Nocturne proof must record a clean source state.", errors)
    require(
        proof.get("runtime", {}).get("runner_mode") == "compiled-product-session",
        "Committed Nocturne proof must use the compiled product-session runner.",
        errors,
    )
    require(
        "products/savers/nocturne/src/nocturne_sim.c" in implementation_paths,
        "Committed Nocturne proof must record the Nocturne simulation source digest.",
        errors,
    )
    require(
        "products/savers/nocturne/src/nocturne_render.c" in implementation_paths,
        "Committed Nocturne proof must record the Nocturne render source digest.",
        errors,
    )


def validate_lifecycle(errors: list[str]) -> None:
    with tempfile.TemporaryDirectory() as output_root:
        lifecycle_dir = pathlib.Path(output_root)
        subprocess.check_call(
            [
                sys.executable,
                str(SSLAB),
                "lifecycle",
                "--product",
                "nocturne",
                "--preset",
                "observatory_night",
                "--width",
                "96",
                "--height",
                "54",
                "--resize-width",
                "80",
                "--resize-height",
                "45",
                "--seed",
                "1536",
                "--frames",
                "8",
                "--delta-ms",
                "100",
                "--output-dir",
                str(lifecycle_dir),
            ],
            cwd=ROOT,
            stdout=subprocess.DEVNULL,
        )
        lifecycle = json.loads((lifecycle_dir / "lifecycle.json").read_text(encoding="utf-8"))
        require(
            lifecycle.get("lifecycle_schema") == "screensave-nocturne-canary-lifecycle-v0",
            "sslab lifecycle must emit screensave-nocturne-canary-lifecycle-v0.",
            errors,
        )
        require(lifecycle.get("status") == "pass", "Nocturne lifecycle proof must pass.", errors)
        require(lifecycle.get("create_session") is True, "Nocturne lifecycle proof must create a product session.", errors)
        require(lifecycle.get("resize_session") is True, "Nocturne lifecycle proof must exercise resize.", errors)
        require(lifecycle.get("step_count") == 8, "Nocturne lifecycle proof must record the step count.", errors)
        require(lifecycle.get("render_session") is True, "Nocturne lifecycle proof must render.", errors)
        require(lifecycle.get("destroy_session") is True, "Nocturne lifecycle proof must destroy the session.", errors)
        require((lifecycle_dir / "lifecycle-capture.ppm").exists(), "Nocturne lifecycle proof must write a capture.", errors)


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
        validate_committed_proof(errors)

    if not errors:
        validate_determinism(errors)

    if not errors:
        validate_lifecycle(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Proof Kernel checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
