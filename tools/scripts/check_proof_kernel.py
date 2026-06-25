"""Validate ScreenSave Proof Kernel v0 scaffolding and deterministic canary output."""

from __future__ import annotations

import json
import hashlib
import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
SSLAB = ROOT / "tools" / "sslab" / "sslab.py"
EVIDENCE_DIR = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne"

EXPECTED_IMPLEMENTATION_PATHS = [
    "tools/sslab/nocturne_canary_runner.c",
    "tools/sslab/src/capture.c",
    "tools/sslab/src/renderer_rgba8.c",
    "products/savers/nocturne/src/nocturne_sim.c",
    "products/savers/nocturne/src/nocturne_render.c",
    "products/savers/nocturne/src/nocturne_themes.c",
    "products/savers/nocturne/src/nocturne_presets.c",
    "platform/src/surface/rgba8/surface_rgba8.c",
    "platform/src/render/soft/soft_renderer.c",
]

REQUIRED_PATHS = [
    ROOT / "contracts" / "proof_kernel_v0.md",
    ROOT / "contracts" / "surface_rgba8_v0.md",
    ROOT / "platform" / "include" / "screensave" / "private" / "surface_rgba8.h",
    ROOT / "platform" / "include" / "screensave" / "private" / "soft_renderer.h",
    ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c",
    ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c",
    ROOT / "tools" / "sslab" / "README.md",
    SSLAB,
    ROOT / "catalog" / "generated" / "proof_registry.json",
    ROOT / "tools" / "sslab" / "nocturne_canary_runner.c",
    ROOT / "tools" / "sslab" / "ricochet_canary_runner.c",
    ROOT / "tools" / "sslab" / "src" / "capture.c",
    ROOT / "tools" / "sslab" / "src" / "capture.h",
    ROOT / "tools" / "sslab" / "src" / "renderer_rgba8.c",
    ROOT / "tools" / "sslab" / "src" / "renderer_rgba8.h",
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
        "PROOF_REGISTRY",
        "sslab-profile-proof-v0",
        "compile_nocturne_runner",
        "compiled-product-session",
        "RUNNER_SOURCES",
        "sha256_file",
        "def render_nocturne",
        "def lifecycle_nocturne",
        "def proof_from_profile",
        "compile_ricochet_runner",
        "proof_ricochet_from_profile",
        "RICOCHET_RUNNER_SOURCES",
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
        "sslab_rgba8_renderer_init",
        "sslab_write_review_ppm",
    ],
    ROOT / "tools" / "sslab" / "ricochet_canary_runner.c": [
        "compiled-ricochet",
        "ricochet_create_session",
        "ricochet_step_session",
        "ricochet_render_session",
        "sslab_write_raw_rgba",
        "frame-%04lu.rgba",
        "sizeof(unsigned long) != 4U",
    ],
    ROOT / "tools" / "sslab" / "src" / "capture.c": [
        "sslab_write_review_ppm",
        "sslab_write_raw_rgba",
        "P3",
    ],
    ROOT / "tools" / "sslab" / "src" / "renderer_rgba8.c": [
        "sslab_rgba8_renderer_init",
        "screensave_renderer_clear",
        "screensave_renderer_draw_polyline",
    ],
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_commit_exists(commit: str) -> bool:
    if not commit:
        return False
    result = subprocess.run(
        ["git", "cat-file", "-e", f"{commit}^{{commit}}"],
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    return result.returncode == 0


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
    implementation = proof.get("implementation", [])
    implementation_by_path = {item.get("path"): item for item in implementation}
    implementation_paths = set(implementation_by_path)
    expected_paths = set(EXPECTED_IMPLEMENTATION_PATHS)
    source_commit = str(proof.get("source", {}).get("commit", ""))

    require(proof.get("source", {}).get("dirty") is False, "Committed Nocturne proof must record a clean source state.", errors)
    require(git_commit_exists(source_commit), "Committed Nocturne proof source commit must exist in this repository.", errors)
    require(
        proof.get("runtime", {}).get("runner_mode") == "compiled-product-session",
        "Committed Nocturne proof must use the compiled product-session runner.",
        errors,
    )
    require(
        implementation_paths == expected_paths,
        "Committed Nocturne proof implementation inputs must exactly match the proof-kernel source set.",
        errors,
    )
    for relative_path in EXPECTED_IMPLEMENTATION_PATHS:
        item = implementation_by_path.get(relative_path, {})
        path = ROOT / relative_path
        require(path.exists(), f"Committed Nocturne proof implementation input is missing: {relative_path}", errors)
        if path.exists():
            require(
                item.get("sha256") == sha256_file(path),
                f"Committed Nocturne proof digest is stale for {relative_path}; regenerate the proof baseline after proof-relevant source changes.",
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


def validate_profile_proof(errors: list[str]) -> None:
    with tempfile.TemporaryDirectory() as output_root:
        profile_dir = pathlib.Path(output_root)
        subprocess.check_call(
            [
                sys.executable,
                str(SSLAB),
                "proof",
                "--profile",
                "nocturne.reference.v0",
                "--output-dir",
                str(profile_dir),
            ],
            cwd=ROOT,
            stdout=subprocess.DEVNULL,
        )
        receipt = json.loads((profile_dir / "profile-proof.json").read_text(encoding="utf-8"))
        require(receipt.get("profile_proof_schema") == "sslab-profile-proof-v0", "sslab proof must emit profile proof schema.", errors)
        require(receipt.get("status") == "pass", "nocturne.reference.v0 profile proof must pass.", errors)
        require(receipt.get("profile") == "nocturne.reference.v0", "profile proof must record the profile key.", errors)
        require(receipt.get("comparison_status") == "pass", "profile proof comparison must pass.", errors)
        require(receipt.get("lifecycle_status") == "pass", "profile proof lifecycle must pass.", errors)
        require(
            receipt.get("render_sha256") == (EVIDENCE_DIR / "capture.sha256").read_text(encoding="ascii").strip(),
            "profile proof must preserve the established Nocturne capture hash.",
            errors,
        )


def validate_ricochet_profile_proof(errors: list[str]) -> None:
    with tempfile.TemporaryDirectory() as output_root:
        profile_dir = pathlib.Path(output_root)
        subprocess.check_call(
            [
                sys.executable,
                str(SSLAB),
                "proof",
                "--profile",
                "ricochet.reference.v1",
                "--output-dir",
                str(profile_dir),
            ],
            cwd=ROOT,
            stdout=subprocess.DEVNULL,
        )
        receipt = json.loads((profile_dir / "profile-proof.json").read_text(encoding="utf-8"))
        captures = receipt.get("captures", [])
        repeat_captures = receipt.get("repeat_captures", [])
        capture_frames = [capture.get("frame") for capture in captures]

        require(receipt.get("profile_proof_schema") == "sslab-profile-proof-v0", "Ricochet profile proof must emit profile proof schema.", errors)
        require(receipt.get("status") == "pass", "ricochet.reference.v1 profile proof must pass.", errors)
        require(receipt.get("profile") == "ricochet.reference.v1", "Ricochet profile proof must record the profile key.", errors)
        require(receipt.get("comparison_status") == "pass", "Ricochet repeated exact comparison must pass.", errors)
        require(receipt.get("lifecycle_status") == "not-run", "Ricochet lifecycle must remain explicit follow-up until generalized lifecycle lands.", errors)
        require(capture_frames == [0, 4, 8, 32], "Ricochet proof must capture frames 0, 4, 8, and 32.", errors)
        require(len(captures) == 4, "Ricochet proof must record four primary captures.", errors)
        require(len(repeat_captures) == 4, "Ricochet proof must record four repeated captures.", errors)

        for capture, repeat_capture in zip(captures, repeat_captures):
            require(
                capture.get("raw_rgba_sha256") == repeat_capture.get("raw_rgba_sha256"),
                f"Ricochet frame {capture.get('frame')} must repeat exactly.",
                errors,
            )
            for key in ("raw_rgba_path", "review_ppm_path"):
                path_text = capture.get(key)
                require(bool(path_text), f"Ricochet capture frame {capture.get('frame')} must record {key}.", errors)
                if path_text:
                    require((ROOT / path_text).exists() or pathlib.Path(path_text).exists(), f"Ricochet capture path must exist: {path_text}", errors)


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

    if not errors:
        validate_profile_proof(errors)

    if not errors:
        validate_ricochet_profile_proof(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Proof Kernel checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
