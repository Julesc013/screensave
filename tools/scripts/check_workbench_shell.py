"""Validate the minimal BenchLab/Workbench shell binding."""

from __future__ import annotations

import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
SHELL_SOURCE = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.c"
SHELL_HEADER = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.h"
SMOKE_SOURCE = ROOT / "products" / "apps" / "benchlab" / "tests" / "workbench_shell_smoke.c"
PROOF_REGISTRY_C = ROOT / "catalog" / "generated" / "proof_registry.c"
SSLAB_HEADER = ROOT / "tools" / "sslab" / "include" / "screensave" / "sslab.h"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in (SHELL_SOURCE, SHELL_HEADER, SMOKE_SOURCE, PROOF_REGISTRY_C, SSLAB_HEADER):
        require(path.exists(), f"Missing Workbench shell path: {path.relative_to(ROOT)}", errors)

    if SHELL_SOURCE.exists():
        text = SHELL_SOURCE.read_text(encoding="utf-8")
        for needle in [
            "catalog",
            "run",
            "inspect",
            "compare",
            "SSLAB_ABI_VERSION",
            "screensave_generated_find_proof_profile",
            "nocturne.reference.v0",
            "ricochet.reference.v1",
        ]:
            require(needle in text, f"benchlab_workbench_shell.c is missing expected text: {needle!r}", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            exe_path = pathlib.Path(temp_root) / "workbench_shell_smoke.exe"
            subprocess.check_call(
                [
                    "gcc",
                    "-std=c89",
                    "-Wall",
                    "-Wextra",
                    "-pedantic",
                    str(SHELL_SOURCE),
                    str(PROOF_REGISTRY_C),
                    str(SMOKE_SOURCE),
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
