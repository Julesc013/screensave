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
PLASMA_HASHES = [
    "bb23057fbf42d3b23a934b1080df971e32877fc43e030957d6b892e60a3886b4",
    "be64bb94300dc792115c4324724660ccd4db488ce5824f7d163594e6cc3a867b",
    "34e272c068df31e32033d5745edb5bf4d371e571b534df5582fb25d2b96db76e",
    "df0c70d4c53e9c9bfb0c7c3eaaa81cad5a53596fff68175ef68cc8b3b504d1af",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_proof(
    runner_path: pathlib.Path,
    profile: str,
    path: str | None = None,
    abi: str | None = None,
) -> dict[str, object]:
    command = [str(runner_path), "proof", "--profile", profile]
    if path is not None:
        command.extend(["--path", path])
    if abi is not None:
        command.extend(["--abi", abi])
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
            plasma_v2 = run_proof(runner, "plasma.v2.reference.preview", "v2", "v1")
            nocturne_captures = nocturne.get("captures", [])
            nocturne_v2_captures = nocturne_v2.get("captures", [])
            ricochet_captures = ricochet.get("captures", [])
            ricochet_v2_captures = ricochet_v2.get("captures", [])
            plasma_v2_captures = plasma_v2.get("captures", [])

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
            require(plasma_v2.get("status") == "pass", "Plasma v2 runner proof must pass.", errors)
            require(plasma_v2.get("path") == "v2", "Plasma proof must record the v2 path.", errors)
            require(plasma_v2.get("abi") == "v1", "Plasma proof must run through ABI v1.", errors)
            require(isinstance(plasma_v2_captures, list) and len(plasma_v2_captures) == 4, "Plasma proof must emit four captures.", errors)
            if isinstance(plasma_v2_captures, list):
                require(
                    [capture.get("rgba_sha256") for capture in plasma_v2_captures] == PLASMA_HASHES,
                    "Plasma v2 preview hashes must remain exact.",
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
