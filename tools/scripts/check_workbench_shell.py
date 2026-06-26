"""Validate the minimal BenchLab/Workbench shell binding."""

from __future__ import annotations

import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "sslab"))

from build_support import INCLUDE_ARGS, LIBSSLAB_SOURCES, PROOF_REGISTRY_C  # noqa: E402

SHELL_SOURCE = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.c"
SHELL_HEADER = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.h"
SMOKE_SOURCE = ROOT / "products" / "apps" / "benchlab" / "tests" / "workbench_shell_smoke.c"
SSLAB_HEADER = ROOT / "tools" / "sslab" / "include" / "screensave" / "sslab.h"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in (SHELL_SOURCE, SHELL_HEADER, SMOKE_SOURCE, PROOF_REGISTRY_C, SSLAB_HEADER, *LIBSSLAB_SOURCES):
        require(path.exists(), f"Missing Workbench shell path: {path.relative_to(ROOT)}", errors)

    if SHELL_SOURCE.exists():
        text = SHELL_SOURCE.read_text(encoding="utf-8")
        for needle in [
            "catalog",
            "run",
            "inspect",
            "compare",
            "author",
            "profile",
            "review",
            "release-readiness",
            "SSLAB_ABI_VERSION",
            "SSLAB_EXECUTION_PATH_V2",
            "screensave_generated_find_proof_profile",
            "benchlab_workbench_shell_run_profile_once",
            "benchlab_workbench_shell_inspect_profile_v2",
            "benchlab_workbench_shell_author_plasma_v2",
            "benchlab_workbench_shell_compare_plasma_v2",
            "benchlab_workbench_shell_profile_plasma_v2",
            "benchlab_workbench_shell_review_plasma_v2",
            "benchlab_workbench_shell_release_readiness_plasma_v2",
            "benchlab_workbench_v2_inspect",
            "benchlab_workbench_plasma_compare",
            "benchlab_workbench_plasma_profile",
            "benchlab_workbench_plasma_review",
            "benchlab_workbench_plasma_release_readiness",
            "ss_v2_product_descriptor_is_valid",
            "nocturne.reference.v0",
            "ricochet.reference.v1",
            "plasma.v2.reference.preview",
            "plasma.v2.visualintent.preview",
            "validation/captures/plasma-v2/matrix/proof-bundles/plasma-v2-visualintent-preview.json",
            "validation/captures/plasma-v2/acceleration/matrix.json",
            "validation/captures/plasma-v2/performance/envelope.json",
            "validation/captures/plasma-v2/stable-candidate-review/review-summary.json",
            "packaging/windows/plasma-v2-preview/manifest.toml",
            "preview-ready",
            "promotion_status",
            "plasma_v2_realization_gl11_candidate",
            "products/savers/plasma/content/v2/examples/plasma_lava_v2.toml",
            "tools/packc/packc.py",
        ]:
            require(needle in text, f"benchlab_workbench_shell.c is missing expected text: {needle!r}", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            temp_path = pathlib.Path(temp_root)
            object_paths: list[pathlib.Path] = []
            for source in LIBSSLAB_SOURCES:
                object_path = temp_path / f"{source.stem}-{len(object_paths)}.o"
                subprocess.check_call(
                    [
                        "gcc",
                        "-std=c89",
                        "-Wall",
                        "-Wextra",
                        "-pedantic",
                        *INCLUDE_ARGS,
                        "-c",
                        str(source),
                        "-o",
                        str(object_path),
                    ],
                    cwd=ROOT,
                )
                object_paths.append(object_path)

            lib_path = temp_path / "libsslab.a"
            subprocess.check_call(
                [
                    "ar",
                    "rcs",
                    str(lib_path),
                    *(str(path) for path in object_paths),
                ],
                cwd=ROOT,
            )

            exe_path = pathlib.Path(temp_root) / "workbench_shell_smoke.exe"
            subprocess.check_call(
                [
                    "gcc",
                    "-std=c89",
                    "-Wall",
                    "-Wextra",
                    "-pedantic",
                    *INCLUDE_ARGS,
                    str(SHELL_SOURCE),
                    str(PROOF_REGISTRY_C),
                    str(SMOKE_SOURCE),
                    str(lib_path),
                    "-o",
                    str(exe_path),
                ],
                cwd=ROOT,
            )
            subprocess.check_call([str(exe_path)], cwd=ROOT)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Workbench shell checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
