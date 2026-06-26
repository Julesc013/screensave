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

    adapter_receipt = load_json(adapter_receipt_path)
    proof_bundle = load_json(proof_bundle_path)
    artifact_manifest = load_json(artifact_manifest_path)
    pe_audit = load_json(pe_audit_path)
    axes = proof_bundle.get("result_axes", {})
    compatibility = axes.get("compatibility", {})
    artistic = axes.get("artistic_review", {})
    release = axes.get("release_promotion", {})
    adapter_payload = adapter_receipt.get("payload", {})

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
            "product": adapter_payload.get("product") or proof_bundle.get("subject", {}).get("product"),
            "preset": adapter_payload.get("preset") or proof_bundle.get("subject", {}).get("preset"),
            "profile": adapter_payload.get("profile") or proof_bundle.get("subject", {}).get("profile"),
        },
        "inputs": {
            "adapter_receipt": repo_path(adapter_receipt_path),
            "proof_bundle": repo_path(proof_bundle_path),
            "artifact_manifest": repo_path(artifact_manifest_path),
            "pe_audit_json": repo_path(pe_audit_path) if pe_audit_path else "",
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
            "artistic_review": axis_claim(
                artistic,
                "Human artistic review is separate from mechanical proof.",
                accepted=False,
            ),
            "release_promotion": axis_claim(
                release,
                "Release promotion is separate from mechanical proof.",
                promoted=False,
            ),
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
    parser.add_argument("--output", required=True)
    parser.set_defaults(func=command_export)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
