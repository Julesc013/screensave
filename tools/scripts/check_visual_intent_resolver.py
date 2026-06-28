"""Validate the deterministic VisualIntent-to-Plasma resolver."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
RESOLVER = ROOT / "tools" / "visualintent" / "resolve_plasma.py"
SCHEMA = ROOT / "tools" / "visualintent" / "schemas" / "visual_intent_plasma_v1.schema.json"
EXAMPLE = ROOT / "tools" / "visualintent" / "examples" / "plasma_warm_room.toml"
CONTRACT = ROOT / "contracts" / "visual_intent_v1.md"
OUT_DIR = ROOT / "out" / "checks" / "visualintent" / "plasma_warm_room"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def run_resolver(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(RESOLVER), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def main() -> int:
    errors: list[str] = []
    for path in [RESOLVER, SCHEMA, EXAMPLE, CONTRACT]:
        require(path.exists(), f"Missing VisualIntent resolver input {path.relative_to(ROOT)}.", errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    contract_text = CONTRACT.read_text(encoding="utf-8")
    for needle in [
        "Product Reduction Law",
        "VisualIntent cannot execute",
        "VisualIntent cannot bypass product-specific schemas",
        "explicit `plasma_v2_spec`",
        "plasma_v2_plan",
        "`candidate_count` defaults to `3`",
        "`candidate_count` must never exceed `5`",
        "candidate rationale must be textual evidence only",
        "generated candidates are review inputs",
    ]:
        require(needle in contract_text, f"VisualIntent contract missing resolver law text: {needle}", errors)

    if OUT_DIR.exists():
        shutil.rmtree(OUT_DIR)
    result = run_resolver(["resolve", str(EXAMPLE), "--out", str(OUT_DIR)])
    require(result.returncode == 0, f"VisualIntent resolver must pass for example input: {result.stderr}", errors)
    summary_path = OUT_DIR / "visualintent-candidates.json"
    require(summary_path.exists(), "VisualIntent resolver must emit visualintent-candidates.json.", errors)
    if summary_path.exists():
        summary = load_json(summary_path)
        require(summary.get("schema_version") == "screensave.visualintent.plasma.resolution.v1", "Resolver summary schema mismatch.", errors)
        require(summary.get("candidate_count") == 3, "Resolver must emit exactly three candidates.", errors)
        require(summary.get("model_calls") is False, "Resolver must record no model calls.", errors)
        require(summary.get("network_calls") is False, "Resolver must record no network calls.", errors)
        require(summary.get("source_code_generation") is False, "Resolver must record no source generation.", errors)
        require(summary.get("automatic_promotion") is False, "Resolver must record no automatic promotion.", errors)
        candidates = summary.get("candidates", [])
        require(len(candidates) == 3, "Resolver candidate array must contain three records.", errors)
        for candidate in candidates:
            for key in ["pack_toml", "compiled_manifest", "hash_manifest", "proof_receipt", "spec"]:
                require(key in candidate, f"Candidate record missing {key}.", errors)
            for key in ["pack_toml", "compiled_manifest", "hash_manifest", "proof_receipt"]:
                ref = candidate.get(key, "")
                require(isinstance(ref, str) and (ROOT / ref).exists(), f"Candidate ref does not exist: {ref}", errors)
            spec = candidate.get("spec", {})
            require(spec.get("schema_id") == "screensave.plasma.spec.v2", "Candidate spec must use Plasma v2 schema.", errors)
            require(spec.get("quality_intent") == "safe", "Candidate spec must keep safe quality intent.", errors)
            require(spec.get("presentation") == "flat", "Candidate spec must keep flat presentation.", errors)
    outside = run_resolver(["resolve", str(EXAMPLE), "--out", str(pathlib.Path("..") / "visualintent-outside")])
    require(outside.returncode != 0, "VisualIntent resolver must reject output outside out/.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("VisualIntent resolver checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
