"""Validate the bounded data-only pack compiler."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
PACKC = ROOT / "tools" / "packc" / "packc.py"
CONTRACT = ROOT / "contracts" / "pack_v1.md"
SCHEMA = ROOT / "tools" / "packc" / "schemas" / "plasma_pack_v1.schema.json"
EXAMPLE = ROOT / "products" / "savers" / "plasma" / "content" / "v2" / "examples" / "plasma_lava_v2.toml"
OUT_DIR = ROOT / "out" / "checks" / "packc" / "plasma_lava_v2"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_packc(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(PACKC), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    errors: list[str] = []
    for path in [PACKC, CONTRACT, SCHEMA, EXAMPLE]:
        require(path.exists(), f"Missing packc input {path.relative_to(ROOT)}.", errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    validate = run_packc(["validate", str(EXAMPLE)])
    require(validate.returncode == 0, "packc validate must pass for the Plasma lava example.", errors)
    if OUT_DIR.exists():
        shutil.rmtree(OUT_DIR)
    compile_run = run_packc(["compile", str(EXAMPLE), "--out", str(OUT_DIR)])
    require(compile_run.returncode == 0, "packc compile must pass for the Plasma lava example.", errors)
    require((OUT_DIR / "manifest.json").exists(), "packc compile must write manifest.json.", errors)
    require((OUT_DIR / "plasma-spec-v2.json").exists(), "packc compile must write plasma-spec-v2.json.", errors)
    require((OUT_DIR / "proof-profile-ref.json").exists(), "packc compile must write proof-profile-ref.json.", errors)
    require((OUT_DIR / "hash-manifest.json").exists(), "packc compile must write hash-manifest.json.", errors)
    if (OUT_DIR / "manifest.json").exists():
        manifest = load_json(OUT_DIR / "manifest.json")
        require(manifest.get("data_only") is True, "packc manifest must record data_only true.", errors)
        require(manifest.get("kind") == "screensave.plasma.v2", "packc manifest must record Plasma pack kind.", errors)
        require(manifest.get("file_count") == 4, "packc manifest must record the bounded file count.", errors)
    if (OUT_DIR / "plasma-spec-v2.json").exists():
        spec = load_json(OUT_DIR / "plasma-spec-v2.json")
        require(spec.get("proof_profile") == "plasma.v2.reference.preview", "compiled pack must reference the preview proof profile.", errors)
        require(spec.get("plasma_spec_v2", {}).get("material", {}).get("token") == "plasma_lava", "compiled pack must resolve plasma_lava.", errors)
    if (OUT_DIR / "hash-manifest.json").exists():
        hashes = load_json(OUT_DIR / "hash-manifest.json")
        require(len(hashes.get("files", [])) == 3, "hash manifest must hash emitted data files.", errors)
        require(hashes.get("total_bytes", 999999) <= hashes.get("max_expanded_bytes", 0), "hash manifest must remain bounded.", errors)

    with tempfile.TemporaryDirectory() as temp_name:
        temp_root = pathlib.Path(temp_name)
        bad_script = temp_root / "bad_script.toml"
        bad_script.write_text(
            EXAMPLE.read_text(encoding="utf-8") + "\n[scripts]\nrun = \"x\"\n",
            encoding="utf-8",
        )
        bad_validate = run_packc(["validate", str(bad_script)])
        require(bad_validate.returncode != 0, "packc must reject script-bearing packs.", errors)
        bad_path = temp_root / "bad_path.toml"
        bad_path.write_text(
            EXAMPLE.read_text(encoding="utf-8").replace(
                'provenance = "PAW-DX U09 deterministic migration"',
                'provenance = "..\\\\escape"',
            ),
            encoding="utf-8",
        )
        bad_path_validate = run_packc(["validate", str(bad_path)])
        require(bad_path_validate.returncode != 0, "packc must reject parent-traversal strings.", errors)
        outside_compile = run_packc(["compile", str(EXAMPLE), "--out", str(temp_root / "outside")])
        require(outside_compile.returncode != 0, "packc compile must reject output outside out/.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("packc checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
