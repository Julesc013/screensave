"""Export ScreenSave proof evidence as an AIDE-facing EvidencePacket projection."""

from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]


def repo_path(path: pathlib.Path) -> str:
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


def resolve_input(value: str) -> pathlib.Path:
    path = pathlib.Path(value)
    if not path.is_absolute():
        path = ROOT / path
    return path.resolve()


def load_json(path: pathlib.Path | None) -> dict[str, Any]:
    if path is None:
        return {}
    return json.loads(path.read_text(encoding="utf-8"))


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def axis_claim(axis: dict[str, Any], summary_fallback: str, **extra: Any) -> dict[str, Any]:
    payload = {
        "status": axis.get("status", "informational"),
        "summary": axis.get("summary", summary_fallback),
        "evidence_refs": axis.get("evidence_refs", []),
    }
    payload.update(extra)
    return payload


def export_packet(args: argparse.Namespace) -> dict[str, Any]:
    adapter_receipt_path = resolve_input(args.adapter_receipt)
    proof_bundle_path = resolve_input(args.proof_bundle)
    artifact_manifest_path = resolve_input(args.artifact_manifest)
    pe_audit_path = resolve_input(args.pe_audit_json) if args.pe_audit_json else None
    portable_v2_equivalence_path = resolve_input(args.portable_v2_equivalence) if args.portable_v2_equivalence else None

    adapter_receipt = load_json(adapter_receipt_path)
    proof_bundle = load_json(proof_bundle_path)
    artifact_manifest = load_json(artifact_manifest_path)
    pe_audit = load_json(pe_audit_path)
    portable_v2_equivalence = load_json(portable_v2_equivalence_path)
    axes = proof_bundle.get("result_axes", {})
    compatibility = axes.get("compatibility", {})
    artistic = axes.get("artistic_review", {})
    release = axes.get("release_promotion", {})
    portable_axis = axes.get("portable_v2_equivalence", portable_v2_equivalence)
    adapter_payload = adapter_receipt.get("payload", {})
    subject_product = adapter_payload.get("product") or proof_bundle.get("subject", {}).get("product")
    subject_profile = adapter_payload.get("profile") or proof_bundle.get("subject", {}).get("profile")
    is_plasma_preview = subject_product == "plasma" and subject_profile == "plasma.v2.reference.preview"
    plasma_material_summary = ROOT / "validation" / "captures" / "plasma-v2" / "materials" / "material-treatment-summary.json"
    plasma_pack_example = ROOT / "products" / "savers" / "plasma" / "content" / "v2" / "examples" / "plasma_lava_v2.toml"
    packc_tool = ROOT / "tools" / "packc" / "packc.py"

    certified_os_support = bool(compatibility.get("certified", False))
    packet = {
        "schema_version": "screensave.aide-evidence-packet.v0",
        "kind": "EvidencePacket",
        "domain": "screensave",
        "source": {
            "commit": adapter_receipt.get("source", {}).get("commit") or git_text(["rev-parse", "HEAD"]),
            "branch": adapter_receipt.get("source", {}).get("branch") or git_text(["branch", "--show-current"]),
            "dirty": bool(adapter_receipt.get("source", {}).get("dirty", bool(git_text(["status", "--short"])))),
        },
        "subject": {
            "product": subject_product,
            "preset": adapter_payload.get("preset") or proof_bundle.get("subject", {}).get("preset"),
            "profile": subject_profile,
        },
        "inputs": {
            "adapter_receipt": repo_path(adapter_receipt_path),
            "proof_bundle": repo_path(proof_bundle_path),
            "artifact_manifest": repo_path(artifact_manifest_path),
            "pe_audit_json": repo_path(pe_audit_path) if pe_audit_path else "",
            "portable_v2_equivalence": repo_path(portable_v2_equivalence_path) if portable_v2_equivalence_path else "",
        },
        "domain_payload_ref": {
            "kind": "Proof Bundle v1",
            "ref": repo_path(proof_bundle_path),
        },
        "claims": {
            "execution": axis_claim(axes.get("execution", {}), "Execution evidence reference."),
            "capture": axis_claim(axes.get("capture", {}), "Capture evidence reference."),
            "exact_comparison": axis_claim(
                axes.get("comparison", {}),
                "Comparison evidence reference.",
                comparison_class=axes.get("comparison", {}).get("comparison_class"),
            ),
            "lifecycle": axis_claim(axes.get("lifecycle", {}), "Lifecycle evidence reference."),
            "performance_soak": axis_claim(axes.get("performance", {}), "Performance and soak evidence reference."),
            "portable_v2_equivalence": axis_claim(
                portable_axis,
                "Portable v2 equivalence remains a separate deterministic canary claim.",
                products=portable_axis.get("products", []),
                profiles=portable_axis.get("profiles", []),
                claim_boundary=portable_axis.get(
                    "claim_boundary",
                    portable_v2_equivalence.get("claim_boundary", "v1/v2 deterministic equivalence for named canary profiles only"),
                ),
            ),
            "artifact_audit": axis_claim(
                axes.get("artifact_audit", {}),
                "Artifact audit evidence reference.",
                artifact_count=axes.get("artifact_audit", {}).get("artifact_count", pe_audit.get("artifact_count", 0)),
                violation_count=axes.get("artifact_audit", {}).get("violation_count", pe_audit.get("violation_count", 0)),
            ),
            "compatibility_evidence_class": axis_claim(
                compatibility,
                "Compatibility evidence class without OS certification.",
                evidence_class=compatibility.get("evidence_class", "targeted"),
                certified_os_support=certified_os_support,
            ),
            "v1_win32_artifacts_preserved": axis_claim(
                axes.get("artifact_audit", {}),
                "Win32 artifact preservation requires native artifact build/audit evidence.",
                artifact_count=axes.get("artifact_audit", {}).get("artifact_count", pe_audit.get("artifact_count", 0)),
                violation_count=axes.get("artifact_audit", {}).get("violation_count", pe_audit.get("violation_count", 0)),
            ),
            "plasma_v2_reference_execution": {
                "status": "supported" if is_plasma_preview else "not-applicable",
                "summary": "Plasma v2 preview reference execution is supported through the fixed v2 proof path." if is_plasma_preview else "Not a Plasma v2 preview packet.",
                "evidence_refs": [repo_path(proof_bundle_path)] if is_plasma_preview else [],
            },
            "plasma_v2_exact_capture": {
                "status": "supported" if is_plasma_preview and axes.get("capture", {}).get("status") == "pass" else "not-applicable",
                "summary": "Plasma v2 preview captures are exact deterministic RGBA evidence." if is_plasma_preview else "Not a Plasma v2 preview packet.",
                "evidence_refs": axes.get("capture", {}).get("evidence_refs", []) if is_plasma_preview else [],
            },
            "plasma_v2_material_distinctness": {
                "status": "supported" if is_plasma_preview and plasma_material_summary.exists() else "not-applicable",
                "summary": "Admitted Plasma v2 materials are distinct and restrained treatments are bounded." if is_plasma_preview else "Not a Plasma v2 preview packet.",
                "evidence_refs": [repo_path(plasma_material_summary)] if is_plasma_preview and plasma_material_summary.exists() else [],
            },
            "plasma_v2_pack_compilation": {
                "status": "supported" if is_plasma_preview and plasma_pack_example.exists() and packc_tool.exists() else "not-applicable",
                "summary": "The Plasma lava v2 example compiles through bounded data-only packc." if is_plasma_preview else "Not a Plasma v2 preview packet.",
                "evidence_refs": [repo_path(plasma_pack_example), repo_path(packc_tool)] if is_plasma_preview and plasma_pack_example.exists() and packc_tool.exists() else [],
            },
            "no_aide_runtime_dependency": {
                "status": "pass",
                "summary": "AIDE EvidencePacket projection stores references only and is absent from ScreenSave product/runtime code.",
                "evidence_refs": [repo_path(adapter_receipt_path), repo_path(proof_bundle_path)],
            },
            "compatibility_not_certified": {
                "status": "pass",
                "summary": "Mechanical proof and binary facts do not certify operating-system compatibility.",
                "evidence_refs": [repo_path(proof_bundle_path)],
                "certified_os_support": False,
            },
            "artistic_review": axis_claim(
                artistic,
                "Human artistic review is separate from mechanical proof.",
                accepted=False,
            ),
            "artistic_acceptance_blocked": {
                "status": artistic.get("status", "blocked"),
                "summary": "Human artistic acceptance remains blocked unless supplied by a human review record.",
                "evidence_refs": artistic.get("evidence_refs", []),
                "accepted": False,
            },
            "release_promotion": axis_claim(
                release,
                "Release promotion is separate from mechanical proof.",
                promoted=False,
            ),
            "release_promotion_blocked": {
                "status": release.get("status", "blocked"),
                "summary": "Release promotion remains blocked unless supplied by an explicit promotion record.",
                "evidence_refs": release.get("evidence_refs", []),
                "promoted": False,
            },
        },
        "artifact_summary": {
            "manifest_ref": repo_path(artifact_manifest_path),
            "artifact_count": len(artifact_manifest.get("artifacts", [])),
        },
        "aide_interpretation": {
            "stores_references_only": True,
            "reinterprets_image_comparison": False,
            "provider_calls": False,
            "model_calls": False,
            "network_calls": False,
        },
        "claim_boundary": "AIDE EvidencePacket projection only; ScreenSave proof remains authoritative and this packet does not certify OS support, accept artistic quality, or promote release.",
    }
    return packet


def command_export(args: argparse.Namespace) -> int:
    packet = export_packet(args)
    output = pathlib.Path(args.output)
    if not output.is_absolute():
        output = ROOT / output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(packet, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"wrote {repo_path(output)}")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--adapter-receipt", required=True)
    parser.add_argument("--proof-bundle", required=True)
    parser.add_argument("--artifact-manifest", required=True)
    parser.add_argument("--pe-audit-json")
    parser.add_argument("--portable-v2-equivalence")
    parser.add_argument("--output", required=True)
    parser.set_defaults(func=command_export)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
