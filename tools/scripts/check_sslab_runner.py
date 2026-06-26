"""Build one private libsslab and validate the generic sslab runner."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
SSLAB_DIR = ROOT / "tools" / "sslab"
if str(SSLAB_DIR) not in sys.path:
    sys.path.insert(0, str(SSLAB_DIR))

from build_support import GENERIC_RUNNER, LIBSSLAB_SOURCES, PROOF_REGISTRY_C, build_runner

NOCTURNE_HASH = "5394a14b6622c17bfb10cd5721c08a4c92cdbddfb12f55c954ef1d5f6ef878b2"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_proof(runner_path: pathlib.Path, profile: str, path: str | None = None) -> dict[str, object]:
    command = [str(runner_path), "proof", "--profile", profile]
    if path is not None:
        command.extend(["--path", path])
    output = subprocess.check_output(
        command,
        cwd=ROOT,
        text=True,
    )
    return json.loads(output)


def main() -> int:
    errors: list[str] = []
    for path in [GENERIC_RUNNER, PROOF_REGISTRY_C, *LIBSSLAB_SOURCES]:
        require(path.exists(), f"Missing sslab runner path: {path.relative_to(ROOT)}", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            runner = build_runner(pathlib.Path(temp_root))
            nocturne = run_proof(runner, "nocturne.reference.v0")
            nocturne_v2 = run_proof(runner, "nocturne.reference.v0", "v2")
            ricochet = run_proof(runner, "ricochet.reference.v1")
            ricochet_v2 = run_proof(runner, "ricochet.reference.v1", "v2")
            nocturne_captures = nocturne.get("captures", [])
            nocturne_v2_captures = nocturne_v2.get("captures", [])
            ricochet_captures = ricochet.get("captures", [])
            ricochet_v2_captures = ricochet_v2.get("captures", [])

            require(nocturne.get("status") == "pass", "Nocturne generic runner proof must pass.", errors)
            require(nocturne.get("path") == "v1", "Nocturne default proof path must remain v1.", errors)
            require(nocturne.get("profile") == "nocturne.reference.v0", "Nocturne proof must record profile key.", errors)
            require(isinstance(nocturne_captures, list) and len(nocturne_captures) == 1, "Nocturne proof must emit one capture.", errors)
            if isinstance(nocturne_captures, list) and nocturne_captures:
                require(
                    nocturne_captures[0].get("rgba_sha256") == NOCTURNE_HASH,
                    "Nocturne generic runner must retain the established RGBA hash.",
                    errors,
                )
            require(nocturne_v2.get("status") == "pass", "Nocturne v2 runner proof must pass.", errors)
            require(nocturne_v2.get("path") == "v2", "Nocturne v2 proof must record path.", errors)
            require(isinstance(nocturne_v2_captures, list) and len(nocturne_v2_captures) == 1, "Nocturne v2 proof must emit one capture.", errors)
            if isinstance(nocturne_v2_captures, list) and nocturne_v2_captures:
                require(
                    nocturne_v2_captures[0].get("rgba_sha256") == NOCTURNE_HASH,
                    "Nocturne v2 runner must retain the established RGBA hash.",
                    errors,
                )

            require(ricochet.get("status") == "pass", "Ricochet generic runner proof must pass.", errors)
            require(ricochet.get("path") == "v1", "Ricochet default proof path must remain v1.", errors)
            require(ricochet.get("profile") == "ricochet.reference.v1", "Ricochet proof must record profile key.", errors)
            require(isinstance(ricochet_captures, list) and len(ricochet_captures) == 4, "Ricochet proof must emit four captures.", errors)
            if isinstance(ricochet_captures, list):
                for capture in ricochet_captures:
                    require(capture.get("status") == "pass", "Each Ricochet capture must pass.", errors)
                    require(len(str(capture.get("rgba_sha256", ""))) == 64, "Each Ricochet capture must emit a SHA-256.", errors)
            lifecycle = ricochet.get("lifecycle", {})
            require(isinstance(lifecycle, dict), "Ricochet proof must emit lifecycle facts.", errors)
            if isinstance(lifecycle, dict):
                require(lifecycle.get("create_destroy_cycles") == 32, "Ricochet lifecycle must run 32 create/destroy cycles.", errors)
            require(ricochet_v2.get("status") == "pass", "Ricochet v2 runner proof must pass.", errors)
            require(ricochet_v2.get("path") == "v2", "Ricochet v2 proof must record path.", errors)
            require(isinstance(ricochet_v2_captures, list) and len(ricochet_v2_captures) == 4, "Ricochet v2 proof must emit four captures.", errors)
            if isinstance(ricochet_captures, list) and isinstance(ricochet_v2_captures, list):
                require(
                    [capture.get("rgba_sha256") for capture in ricochet_v2_captures]
                    == [capture.get("rgba_sha256") for capture in ricochet_captures],
                    "Ricochet v2 runner hashes must match the exact v1 frame hashes.",
                    errors,
                )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Generic sslab runner checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
