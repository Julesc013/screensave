"""Validate the ScreenSave AIDE evidence bridge exporter."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
EXPORTER = ROOT / "tools" / "aide_bridge" / "export_evidence.py"
SCHEMA = ROOT / "tools" / "aide_bridge" / "schemas" / "screensave_evidence_packet_v0.schema.json"
README = ROOT / "tools" / "aide_bridge" / "README.md"
ADAPTER = ROOT / "tools" / "project_adapter" / "screensave_project.py"
OUT = ROOT / "out" / "aide" / "evidence-bridge-check"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def run(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def main() -> int:
    errors: list[str] = []
    for path in [EXPORTER, SCHEMA, README, ADAPTER]:
        require(path.exists(), f"Missing evidence bridge path: {path.relative_to(ROOT)}", errors)

    if not errors:
        bundle = run([
            str(ADAPTER.relative_to(ROOT)),
            "bundle",
            "--invocation-id",
            "check-evidence-bridge",
            "--profile",
            "ricochet.reference.v1",
        ])
        require(bundle.returncode == 0, f"adapter bundle failed: {bundle.stderr or bundle.stdout}", errors)
        base = ROOT / "out" / "aide" / "screensave-project-adapter" / "invocations" / "bundle" / "check-evidence-bridge"
        adapter_receipt = base / "adapter-proof.json"
        proof_bundle = base / "proof-bundle-v1.json"
        artifact_manifest = base / "artifact-manifest.json"
        packet_path = OUT / "ricochet.evidence-packet.json"
        export = run([
            str(EXPORTER.relative_to(ROOT)),
            "--adapter-receipt",
            str(adapter_receipt),
            "--proof-bundle",
            str(proof_bundle),
            "--artifact-manifest",
            str(artifact_manifest),
            "--output",
            str(packet_path),
        ])
        require(export.returncode == 0, f"evidence export failed: {export.stderr or export.stdout}", errors)
        require(packet_path.exists(), "evidence exporter must write the packet.", errors)
        if packet_path.exists():
            packet = load_json(packet_path)
            claims = packet.get("claims", {})
            schema = load_json(SCHEMA)
            require(packet.get("schema_version") == "screensave.aide-evidence-packet.v0", "packet schema mismatch.", errors)
            require(packet.get("kind") == "EvidencePacket", "packet kind must be EvidencePacket.", errors)
            require(packet.get("domain") == "screensave", "packet domain must be screensave.", errors)
            require(set(schema.get("required_claims", [])) <= set(claims), "packet missing required claim axes.", errors)
            require(claims.get("capture", {}).get("status") == "pass", "capture claim must pass for Ricochet check.", errors)
            require(claims.get("exact_comparison", {}).get("status") == "pass", "comparison claim must pass for Ricochet check.", errors)
            require(claims.get("lifecycle", {}).get("status") == "pass", "lifecycle claim must pass for Ricochet check.", errors)
            require(
                claims.get("compatibility_evidence_class", {}).get("certified_os_support") is False,
                "EvidencePacket must not certify OS support from proof alone.",
                errors,
            )
            require(
                claims.get("artistic_review", {}).get("status") == "blocked",
                "artistic review must stay blocked unless human evidence is supplied.",
                errors,
            )
            require(
                claims.get("release_promotion", {}).get("status") == "blocked",
                "release promotion must stay blocked unless promotion evidence is supplied.",
                errors,
            )
            require(
                packet.get("aide_interpretation", {}).get("reinterprets_image_comparison") is False,
                "AIDE projection must not reinterpret image comparison.",
                errors,
            )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("AIDE evidence bridge checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
