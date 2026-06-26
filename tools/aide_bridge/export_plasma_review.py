"""Export Plasma v2 review evidence as a reference-only AIDE packet."""

from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def resolve(path_text: str) -> pathlib.Path:
    path = pathlib.Path(path_text)
    if not path.is_absolute():
        path = ROOT / path
    return path.resolve()


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def claim(status: str, summary: str, refs: list[str]) -> dict[str, Any]:
    return {
        "status": status,
        "summary": summary,
        "evidence_refs": refs,
    }


def export_packet(ledger_path: pathlib.Path) -> dict[str, Any]:
    ledger = load_toml(ledger_path)
    refs = ledger.get("refs", {})
    claims = ledger.get("claims", {})
    material = load_json(resolve(str(refs.get("material_matrix", ""))))
    controls = load_json(resolve(str(refs.get("control_matrix", ""))))
    candidates = load_json(resolve(str(refs.get("visualintent_candidates", ""))))
    review_protocol = load_json(resolve(str(refs.get("visual_review_protocol", ""))))

    return {
        "schema_version": "screensave.aide-plasma-review-evidence.v0",
        "kind": "EvidencePacket",
        "domain": "screensave",
        "subject": {
            "product": ledger.get("product"),
            "profile": ledger.get("profile"),
            "status": "review-evidence",
        },
        "source": {
            "commit": git_text(["rev-parse", "HEAD"]),
            "branch": git_text(["branch", "--show-current"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "inputs": {
            "ledger": repo_path(ledger_path),
            "proof_matrix": refs.get("proof_matrix", ""),
            "visual_review_protocol": refs.get("visual_review_protocol", ""),
        },
        "claims": {
            "plasma_v2_reference_execution": claim(
                str(claims.get("plasma_v2_reference_execution", "")),
                "Plasma v2 reference execution is supported by committed preview proof.",
                [refs.get("reference_proof", ""), refs.get("reference_bundle", "")],
            ),
            "pack_compilation": claim(
                str(claims.get("pack_compilation", "")),
                "Data-only pack compilation is supported for the preview example.",
                [refs.get("pack_example", "")],
            ),
            "candidate_generation": claim(
                str(claims.get("candidate_generation", "")),
                "VisualIntent resolver emits three deterministic bounded candidates.",
                [refs.get("visualintent_candidates", "")],
            ),
            "control_influence": claim(
                str(claims.get("control_influence", "")),
                "Basic controls materially influence output except explicit metadata-only fields.",
                [refs.get("control_matrix", "")],
            ),
            "material_distinctness": claim(
                str(claims.get("material_distinctness", "")),
                "Five admitted materials and three restrained treatments are covered.",
                [refs.get("material_matrix", "")],
            ),
            "visual_review": claim(
                str(claims.get("visual_review", "")),
                "Visual review protocol and decision template are recorded; final human acceptance is separate.",
                [refs.get("visual_review_protocol", ""), refs.get("reviewer_decision_template", "")],
            ),
            "artistic_acceptance": claim(
                str(claims.get("artistic_acceptance", "")),
                "Final artistic acceptance has not been granted by this packet.",
                [refs.get("visual_review_protocol", "")],
            ),
            "stable_release_promotion": claim(
                str(claims.get("stable_release_promotion", "")),
                "Stable release promotion remains blocked.",
                [],
            ),
            "compatibility_certification": claim(
                str(claims.get("compatibility_certification", "")),
                "Compatibility certification is not claimed by this review evidence.",
                [],
            ),
            "aide_runtime_dependency": {
                "status": "false",
                "summary": "AIDE remains development-plane evidence only and is not a product/runtime dependency.",
                "evidence_refs": [repo_path(ledger_path)],
                "value": bool(claims.get("aide_runtime_dependency")),
            },
        },
        "review_facts": {
            "decision_classes": review_protocol.get("decision_classes", []),
            "material_count": material.get("material_count"),
            "treatment_count": material.get("treatment_count"),
            "materially_influencing_controls": controls.get("materially_influencing_controls"),
            "candidate_count": candidates.get("candidate_count"),
            "model_calls": candidates.get("model_calls"),
            "network_calls": candidates.get("network_calls"),
            "automatic_promotion": candidates.get("automatic_promotion"),
        },
        "aide_interpretation": {
            "stores_references_only": True,
            "provider_calls": False,
            "model_calls": False,
            "network_calls": False,
            "reinterprets_image_comparison": False,
        },
        "claim_boundary": ledger.get("claim_boundary"),
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ledger", default=".aide/evidence/plasma-v2-review.toml")
    parser.add_argument("--output", default="out/aide/evidence/plasma-v2-review-packet.json")
    args = parser.parse_args(argv)

    ledger_path = resolve(args.ledger)
    output = resolve(args.output)
    packet = export_packet(ledger_path)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(packet, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"wrote {repo_path(output)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
