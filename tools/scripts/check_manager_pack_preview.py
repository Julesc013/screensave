"""Validate the Plasma v2 Manager pack preview model."""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
HEADER = ROOT / "products" / "apps" / "suite" / "src" / "manager_pack_preview.h"
SOURCE = ROOT / "products" / "apps" / "suite" / "src" / "manager_pack_preview.c"
SMOKE = ROOT / "products" / "apps" / "suite" / "tests" / "manager_pack_preview_smoke.c"
PACKAGE_MANIFEST = ROOT / "packaging" / "windows" / "plasma-v2-preview" / "manifest.toml"
PACK_EXAMPLE = ROOT / "products" / "savers" / "plasma" / "content" / "v2" / "examples" / "plasma_lava_v2.toml"
PROOF_BUNDLE = ROOT / "validation" / "captures" / "plasma-v2" / "reference-preview" / "proof-bundle-v1.json"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def compile_smoke(errors: list[str]) -> None:
    compiler = shutil.which("gcc")
    if compiler is None:
        errors.append("gcc must be available for the Manager pack preview smoke compile.")
        return

    with tempfile.TemporaryDirectory() as temp_name:
        exe = pathlib.Path(temp_name) / "manager_pack_preview_smoke.exe"
        result = subprocess.run(
            [
                compiler,
                "-std=c89",
                "-Wall",
                "-Wextra",
                str(SMOKE),
                str(SOURCE),
                "-o",
                str(exe),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        require(result.returncode == 0, "Manager pack preview smoke compile failed:\n" + result.stderr.strip(), errors)
        if result.returncode == 0:
            run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            require(run.returncode == 0, "Manager pack preview smoke execution failed.", errors)


def main() -> int:
    errors: list[str] = []
    for path in (HEADER, SOURCE, SMOKE, PACKAGE_MANIFEST, PACK_EXAMPLE, PROOF_BUNDLE):
        require(path.exists(), f"Missing Manager pack preview input {path.relative_to(ROOT)}.", errors)

    if not errors:
        header = read_text(HEADER)
        source = read_text(SOURCE)
        smoke = read_text(SMOKE)
        for snippet in (
            "manager_pack_preview_entry",
            "pack_manifest_ref",
            "provenance",
            "license",
            "product_compatibility",
            "installability_status",
            "refusal_reason",
            "proof_bundle_ref",
            "rollback_note",
            "claim_boundary",
        ):
            require(snippet in header, f"manager_pack_preview.h is missing {snippet!r}.", errors)
        for snippet in (
            "plasma.v2.reference.preview",
            "ScreenSave sample content",
            "real pack installation mutation is not admitted",
            "compatibility certification",
            "install_mutation_allowed = 0",
        ):
            require(snippet in source, f"manager_pack_preview.c is missing {snippet!r}.", errors)
        for snippet in (
            "manager_pack_preview_plasma_v2",
            "manager_pack_preview_is_installable",
            "manager_pack_preview_refusal_reason",
        ):
            require(snippet in smoke, f"manager_pack_preview_smoke.c is missing {snippet!r}.", errors)
        compile_smoke(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Manager pack preview checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
