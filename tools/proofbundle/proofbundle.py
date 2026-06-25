"""Normalize ScreenSave proof receipts into Proof Bundle v1 JSON."""

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


def load_json(path: pathlib.Path | None) -> dict[str, Any]:
    if path is None:
        return {}
    return json.loads(path.read_text(encoding="utf-8"))


def resolve_input(value: str | None) -> pathlib.Path | None:
    if value is None:
        return None
    path = pathlib.Path(value)
    if not path.is_absolute():
        path = ROOT / path
    return path.resolve()


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def source_payload(proof: dict[str, Any]) -> dict[str, Any]:
    source = proof.get("source", {})
    return {
        "commit": source.get("commit", git_text(["rev-parse", "HEAD"])),
        "dirty": bool(source.get("dirty", bool(git_text(["status", "--short"])))),
        "bundle_generated_at_source": git_text(["rev-parse", "HEAD"]),
    }


def axis(status: str, summary: str, evidence_refs: list[str] | None = None, **extra: Any) -> dict[str, Any]:
    payload = {
        "status": status,
        "summary": summary,
        "evidence_refs": evidence_refs or [],
    }
    payload.update(extra)
    return payload


def status_from_input(payload: dict[str, Any], default: str = "informational") -> str:
    status = str(payload.get("status", default))
    if status in {"pass", "fail", "blocked", "informational"}:
        return status
    return default


def normalize(args: argparse.Namespace) -> dict[str, Any]:
    proof_path = resolve_input(args.proof)
    comparison_path = resolve_input(args.comparison)
    audit_path = resolve_input(args.pe_audit_json)
    build_path = resolve_input(args.build_receipt)
    adapter_path = resolve_input(args.adapter_receipt)
    lifecycle_path = resolve_input(args.lifecycle)
    performance_path = resolve_input(args.performance)

    proof = load_json(proof_path)
    comparison = load_json(comparison_path)
    pe_audit = load_json(audit_path)
    build_receipt = load_json(build_path)
    adapter_receipt = load_json(adapter_path)
    lifecycle = load_json(lifecycle_path)
    performance = load_json(performance_path)

    runtime = proof.get("runtime", {})
    capture = proof.get("capture", {})
    product = runtime.get("product") or adapter_receipt.get("payload", {}).get("profile") or "unknown"
    proof_ref = repo_path(proof_path) if proof_path else ""
    comparison_ref = repo_path(comparison_path) if comparison_path else ""
    audit_ref = repo_path(audit_path) if audit_path else ""
    build_ref = repo_path(build_path) if build_path else ""
    adapter_ref = repo_path(adapter_path) if adapter_path else ""
    lifecycle_ref = repo_path(lifecycle_path) if lifecycle_path else ""
    performance_ref = repo_path(performance_path) if performance_path else ""

    artifact_count = int(pe_audit.get("artifact_count", 0)) if pe_audit else 0
    pe_audit_status = status_from_input(pe_audit) if pe_audit else "informational"
    compatibility_class = "targeted"
    if artifact_count > 0 and pe_audit_status in {"pass", "informational"}:
        compatibility_class = "binary-audited"

    comparison_status = status_from_input(comparison) if comparison else "informational"
    lifecycle_status = status_from_input(lifecycle) if lifecycle else "informational"
    performance_status = status_from_input(performance) if performance else "informational"
    build_status = status_from_input(build_receipt) if build_receipt else "informational"
    execution_status = "fail" if proof.get("status") == "fail" or build_status == "fail" else "informational"
    if build_status == "blocked":
        execution_status = "blocked"

    bundle = {
        "proof_schema": "proof-bundle-v1",
        "status": "informational",
        "source": source_payload(proof),
        "subject": {
            "product": product,
            "preset": runtime.get("preset"),
            "profile": build_receipt.get("profile"),
            "proof_kernel": proof.get("proof_kernel"),
        },
        "inputs": {
            "proof": proof_ref,
            "comparison": comparison_ref,
            "pe_audit_json": audit_ref,
            "build_receipt": build_ref,
            "adapter_receipt": adapter_ref,
            "lifecycle": lifecycle_ref,
            "performance": performance_ref,
        },
        "result_axes": {
            "execution": axis(
                execution_status,
                "Execution evidence was normalized from proof and optional build receipts.",
                [ref for ref in [proof_ref, build_ref, adapter_ref] if ref],
                runner=runtime.get("runner"),
                runner_mode=runtime.get("runner_mode"),
                build_status=build_status,
            ),
            "capture": axis(
                "pass" if capture.get("sha256") else "blocked",
                "Capture hash is present." if capture.get("sha256") else "No capture hash was supplied.",
                [proof_ref] if proof_ref else [],
                capture_ref=capture.get("path"),
                capture_sha256=capture.get("sha256"),
                canonical_hash_source=capture.get("canonical_hash_source"),
            ),
            "comparison": axis(
                comparison_status,
                "Comparison receipt was supplied." if comparison else "No comparison receipt was supplied.",
                [comparison_ref] if comparison_ref else [],
                comparison_class=comparison.get("class"),
                metrics=comparison.get("metrics", {}),
            ),
            "lifecycle": axis(
                lifecycle_status,
                "Lifecycle receipt was supplied." if lifecycle else "No lifecycle receipt was supplied.",
                [lifecycle_ref] if lifecycle_ref else [],
                lifecycle_schema=lifecycle.get("lifecycle_schema") or lifecycle.get("schema"),
                create_session=lifecycle.get("create_session"),
                resize_session=lifecycle.get("resize_session"),
                step_count=lifecycle.get("step_count"),
                render_session=lifecycle.get("render_session"),
                destroy_session=lifecycle.get("destroy_session"),
                width=lifecycle.get("width"),
                height=lifecycle.get("height"),
                checksum=lifecycle.get("checksum"),
            ),
            "performance": axis(
                performance_status,
                "Performance receipt was supplied." if performance else "No performance receipt was supplied.",
                [performance_ref] if performance_ref else [],
                performance_schema=performance.get("performance_schema") or performance.get("schema"),
                frame_count=performance.get("frame_count"),
                frame_time_ms=performance.get("frame_time_ms", {}),
                memory=performance.get("memory", {}),
                handles=performance.get("handles", {}),
                soak=performance.get("soak", {}),
                limits=performance.get("limits", {}),
            ),
            "artifact_audit": axis(
                pe_audit_status,
                "PE audit facts were supplied." if pe_audit else "No PE audit facts were supplied.",
                [audit_ref] if audit_ref else [],
                artifact_count=artifact_count,
                violation_count=int(pe_audit.get("violation_count", 0)) if pe_audit else 0,
                artifact_profile=pe_audit.get("artifact_profile"),
                artifact_manifest=pe_audit.get("artifact_manifest"),
            ),
            "compatibility": axis(
                "informational",
                "Compatibility evidence class is derived without certifying an operating system.",
                [audit_ref] if audit_ref else [],
                evidence_class=compatibility_class,
                certified=False,
            ),
            "artistic_review": axis(
                "blocked",
                "Human visual-artistic review has not been supplied.",
                [],
            ),
            "release_promotion": axis(
                "blocked",
                "Release promotion has not been requested or approved.",
                [],
            ),
        },
        "claim_boundary": "Normalized proof evidence only; not compatibility certification, artistic acceptance, or release promotion.",
        "limits": [
            "Proof Bundle v1 separates result axes but does not rerun proof tools.",
            "A blocked artistic-review axis is not a mechanical proof failure.",
            "A binary-audited compatibility class is not certified OS support.",
        ],
    }
    return bundle


def command_normalize(args: argparse.Namespace) -> int:
    bundle = normalize(args)
    output = pathlib.Path(args.output)
    if not output.is_absolute():
        output = ROOT / output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(bundle, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"{bundle['status']} {repo_path(output)}")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    normalize_parser = subparsers.add_parser("normalize", help="Write a Proof Bundle v1 envelope.")
    normalize_parser.add_argument("--proof", required=True, help="Proof JSON input.")
    normalize_parser.add_argument("--comparison", help="Optional comparison JSON input.")
    normalize_parser.add_argument("--pe-audit-json", help="Optional PE audit JSON input.")
    normalize_parser.add_argument("--build-receipt", help="Optional build receipt JSON input.")
    normalize_parser.add_argument("--adapter-receipt", help="Optional adapter receipt JSON input.")
    normalize_parser.add_argument("--lifecycle", help="Optional lifecycle JSON input.")
    normalize_parser.add_argument("--performance", help="Optional performance/profile JSON input.")
    normalize_parser.add_argument("--output", required=True)
    normalize_parser.set_defaults(func=command_normalize)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
