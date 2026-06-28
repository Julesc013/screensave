"""Validate Plasma v2 publication-prep readiness."""

from __future__ import annotations

import hashlib
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
RELEASE_DIR = ROOT / "releases" / "plasma-v2-stable"
MANIFEST = RELEASE_DIR / "release-manifest.toml"
ARTIFACT_MANIFEST = RELEASE_DIR / "artifact-manifest.toml"
CHECKSUMS = RELEASE_DIR / "checksums.sha256"
PROVENANCE = RELEASE_DIR / "provenance.json"
KNOWN_LIMITS = RELEASE_DIR / "known-limits.md"
SUPPORT_MATRIX = RELEASE_DIR / "support-matrix.md"
INSTALL_NOTES = RELEASE_DIR / "install-notes.md"
ROLLBACK_NOTES = RELEASE_DIR / "rollback-notes.md"
THIRD_PARTY = RELEASE_DIR / "third-party-notices.md"
CHECKLIST = RELEASE_DIR / "publication-checklist.md"
GATE_REPORT = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "gate-report.json"
GATE_REPORT_MD = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "gate-report.md"
STABLE_GATE = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "gate-report.json"
AIDE_SUMMARY = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "aide-publication-summary.json"

SUBCHECKS = [
    ("publication-prep-contract", [sys.executable, "tools/scripts/check_plasma_v2_publication_prep_contract.py"]),
    ("release-provenance", [sys.executable, "tools/scripts/check_release_provenance.py"]),
    ("release-security", [sys.executable, "tools/scripts/check_release_security.py"]),
    ("package-stage", [sys.executable, "tools/scripts/check_plasma_v2_package_stage.py"]),
    ("aide-evidence-index", [sys.executable, "tools/scripts/check_aide_evidence_index.py"]),
]


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_text(args: list[str]) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return result.stdout.strip()


def run(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    return {
        "command": " ".join(command),
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
    }


def add_check(checks: list[dict[str, Any]], check_id: str, ok: bool, message: str, **details: Any) -> None:
    payload: dict[str, Any] = {
        "id": check_id,
        "status": "pass" if ok else "fail",
        "message": message,
    }
    payload.update(details)
    checks.append(payload)


def parse_sha256_lines(path: pathlib.Path, checks: list[dict[str, Any]]) -> dict[str, str]:
    values: dict[str, str] = {}
    if not path.exists():
        return values
    errors: list[str] = []
    for line_number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        stripped = line.strip()
        if not stripped:
            continue
        parts = stripped.split(None, 1)
        if len(parts) != 2:
            errors.append(f"line {line_number} is not '<sha256> <path>'")
            continue
        digest, ref = parts
        if len(digest) != 64 or any(char not in "0123456789abcdef" for char in digest):
            errors.append(f"line {line_number} has invalid sha256: {digest}")
            continue
        values[ref.strip()] = digest
    add_check(
        checks,
        "checksum-file-format",
        not errors and bool(values),
        "Publication-prep checksum file is parseable.",
        errors=errors,
        count=len(values),
    )
    return values


def check_path_set(checks: list[dict[str, Any]]) -> None:
    required = [
        MANIFEST,
        ARTIFACT_MANIFEST,
        CHECKSUMS,
        PROVENANCE,
        KNOWN_LIMITS,
        SUPPORT_MATRIX,
        INSTALL_NOTES,
        ROLLBACK_NOTES,
        THIRD_PARTY,
        CHECKLIST,
        AIDE_SUMMARY,
    ]
    missing = [str(path.relative_to(ROOT)) for path in required if not path.exists()]
    add_check(checks, "publication-packet-paths", not missing, "Publication-prep packet paths exist.", missing=missing)


def check_project_state(checks: list[dict[str, Any]]) -> None:
    if not STATE.exists():
        add_check(checks, "project-state", False, "PROJECT_STATE.toml is missing.")
        return
    state = load_toml(STATE)
    authority = state.get("authority", {})
    development = state.get("development", {})
    plasma = state.get("plasma_v2", {})
    status = plasma.get("status")
    active_program = authority.get("active_program")
    expected_active_programs = {
        "stable-promoted": "plasma-v2-publication-prep",
        "publication-ready": "plasma-v2-publication",
    }
    expected_active = expected_active_programs.get(str(status), "")
    ok = (
        status in expected_active_programs
        and plasma.get("stable") is True
        and plasma.get("release_promotion") == "accepted"
        and plasma.get("release_candidate") == "plasma-v2-rc1"
        and authority.get("release_candidate") == "plasma-v2-rc1"
        and active_program == expected_active
        and development.get("active_program") == expected_active
    )
    add_check(
        checks,
        "project-state",
        ok,
        "Project state is stable-promoted for prep or publication-ready after prep.",
        plasma_status=status,
        active_program=active_program,
        expected_active_program=expected_active,
        stable=plasma.get("stable"),
        release_promotion=plasma.get("release_promotion"),
    )


def check_manifests(checks: list[dict[str, Any]]) -> None:
    if MANIFEST.exists():
        manifest = load_toml(MANIFEST)
        publication = manifest.get("publication", {})
        ok = (
            manifest.get("status") == "publication-prep"
            and manifest.get("publication_status") == "not-published"
            and manifest.get("release_candidate") == "plasma-v2-rc1"
            and manifest.get("profile") == "plasma.v2.reference.preview"
            and manifest.get("stable_promotion") == "accepted"
            and manifest.get("compatibility_certification") == "not-broadened"
            and manifest.get("public_sdk_stability") == "not-claimed"
            and manifest.get("all_saver_migration") == "not-claimed"
            and publication.get("public_upload") is False
            and publication.get("release_page_published") is False
            and publication.get("github_release_created") is False
            and publication.get("download_links_verified") is False
            and publication.get("publication_receipt") == "not-created"
        )
        add_check(checks, "release-manifest", ok, "Release manifest records publication-prep only.", manifest=manifest)
        for section, refs in [
            ("artifacts", manifest.get("artifacts", {})),
            ("evidence", manifest.get("evidence", {})),
            ("documents", manifest.get("documents", {})),
        ]:
            missing = [ref for ref in refs.values() if isinstance(ref, str) and not (ROOT / ref).exists()]
            add_check(checks, f"release-manifest-{section}-refs", not missing, f"Release manifest {section} refs exist.", missing=missing)
    if ARTIFACT_MANIFEST.exists():
        artifacts = load_toml(ARTIFACT_MANIFEST)
        rows = artifacts.get("artifacts", [])
        published = [row.get("id") for row in rows if isinstance(row, dict) and row.get("published") is not False]
        artifact_paths = [row.get("path") for row in rows if isinstance(row, dict)]
        add_check(
            checks,
            "artifact-manifest",
            artifacts.get("status") == "publication-prep"
            and artifacts.get("publication_status") == "not-published"
            and artifacts.get("release_candidate") == "plasma-v2-rc1"
            and not published
            and bool(rows),
            "Artifact manifest records unpublished publication-prep artifacts.",
            published=published,
            artifact_paths=artifact_paths,
        )
        missing = [path for path in artifact_paths if isinstance(path, str) and not (ROOT / path).exists()]
        add_check(checks, "artifact-manifest-refs", not missing, "Artifact manifest paths exist.", missing=missing)


def check_checksums(checks: list[dict[str, Any]]) -> None:
    values = parse_sha256_lines(CHECKSUMS, checks)
    mismatches: list[str] = []
    missing: list[str] = []
    for ref, expected in values.items():
        path = ROOT / ref
        if not path.exists():
            missing.append(ref)
            continue
        actual = sha256_file(path)
        if actual != expected:
            mismatches.append(f"{ref}: expected {expected}, actual {actual}")
    add_check(
        checks,
        "checksums-match",
        not missing and not mismatches,
        "Publication-prep checksums match referenced files.",
        missing=missing,
        mismatches=mismatches,
    )


def check_documents(checks: list[dict[str, Any]]) -> None:
    doc_needles = {
        KNOWN_LIMITS: [
            "does not publish a release",
            "broaden compatibility certification",
            "GDI remains mandatory",
            "GL11 remains optional",
            "Compatibility certification is not broadened",
        ],
        SUPPORT_MATRIX: [
            "artifact-specific",
            "not broad compatibility certification",
            "GDI presentation",
            "GL11 candidate",
            "All Core savers",
        ],
        INSTALL_NOTES: [
            "does not perform installation",
            "does not perform installation",
            "Do not infer compatibility certification",
            "Manager preview evidence",
        ],
        ROLLBACK_NOTES: [
            "no installer side effects",
            "publishes nothing",
            "Compatibility certification is not broadened",
        ],
        THIRD_PARTY: [
            "does not add mandatory third-party runtime dependencies",
            "data-only",
            "runtime executable plugins",
        ],
        CHECKLIST: [
            "Status: publication-prep, not published.",
            "Public release publication.",
            "Release asset upload.",
            "Compatibility certification broadening.",
            "Graphical Workbench MVP.",
        ],
    }
    for path, needles in doc_needles.items():
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        missing = [needle for needle in needles if needle not in text]
        add_check(checks, f"doc-{path.stem}", not missing, f"{path.relative_to(ROOT)} records required publication-prep wording.", missing=missing)


def check_provenance(checks: list[dict[str, Any]]) -> None:
    if PROVENANCE.exists():
        provenance = load_json(PROVENANCE)
        artifact = provenance.get("artifact", {})
        artifact_path = ROOT / str(artifact.get("path", ""))
        artifact_ok = artifact_path.exists() and sha256_file(artifact_path) == artifact.get("sha256")
        boundaries = provenance.get("boundaries", {})
        add_check(
            checks,
            "publication-provenance",
            provenance.get("status") == "publication-prep"
            and provenance.get("publication_status") == "not-published"
            and artifact_ok
            and boundaries.get("public_upload") is False
            and boundaries.get("release_page_published") is False
            and boundaries.get("compatibility_certification_broadened") is False
            and boundaries.get("public_sdk_stability") == "not claimed"
            and boundaries.get("all_saver_migration") == "not claimed",
            "Publication-prep provenance records artifact and boundary facts.",
            artifact_ok=artifact_ok,
            boundaries=boundaries,
        )
        missing_refs = [ref for ref in provenance.get("evidence_refs", []) if not (ROOT / ref).exists()]
        add_check(checks, "publication-provenance-refs", not missing_refs, "Publication-prep provenance refs exist.", missing=missing_refs)


def check_stable_gate_and_aide(checks: list[dict[str, Any]]) -> None:
    stable_gate = load_json(STABLE_GATE) if STABLE_GATE.exists() else {}
    add_check(
        checks,
        "stable-promotion-accepted",
        stable_gate.get("status") == "promotion-ready" and stable_gate.get("recommended_state") == "stable-promoted",
        "Stable-promotion gate remains accepted before publication prep.",
        stable_gate_status=stable_gate.get("status"),
        recommended_state=stable_gate.get("recommended_state"),
    )
    summary = load_json(AIDE_SUMMARY) if AIDE_SUMMARY.exists() else {}
    assertions = summary.get("aide_assertions", {})
    add_check(
        checks,
        "aide-publication-summary",
        summary.get("status") == "pass"
        and summary.get("stable_promotion") == "accepted"
        and summary.get("publication_prep") == "recorded"
        and summary.get("publication_upload") == "not-performed"
        and summary.get("release_page_publication") == "not-performed"
        and summary.get("compatibility_certification_broadening") is False
        and summary.get("aide_runtime_dependency") is False
        and all(assertions.get(key) is True for key in [
            "did_not_publish_release",
            "did_not_certify_compatibility",
            "did_not_promote_release",
            "did_not_mutate_source_automatically",
            "did_not_become_runtime_dependency",
        ]),
        "AIDE publication summary records evidence-only posture.",
        summary=summary,
    )


def check_no_publication_side_effects(checks: list[dict[str, Any]]) -> None:
    forbidden_paths = [
        RELEASE_DIR / "publication-receipt.json",
        RELEASE_DIR / "github-release.json",
        RELEASE_DIR / "uploaded-assets.json",
    ]
    existing = [str(path.relative_to(ROOT)) for path in forbidden_paths if path.exists()]
    add_check(checks, "no-publication-receipts", not existing, "No publication/upload receipt exists in PAW-J.", existing=existing)


def build_report() -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    check_path_set(checks)
    check_project_state(checks)
    check_manifests(checks)
    check_checksums(checks)
    check_documents(checks)
    check_provenance(checks)
    check_stable_gate_and_aide(checks)
    check_no_publication_side_effects(checks)

    for check_id, command in SUBCHECKS:
        result = run(command)
        add_check(
            checks,
            check_id,
            result.get("returncode") == 0,
            "Command passed." if result.get("returncode") == 0 else "Command failed.",
            run=result,
        )

    status = "publication-ready" if all(item.get("status") == "pass" for item in checks) else "publication-hold"
    return {
        "schema": "screensave.plasma-v2.publication-prep.gate-report.v1",
        "status": status,
        "candidate_id": "plasma-v2-rc1",
        "recommended_state": status,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "checks": checks,
        "claim_boundary": "Publication-prep gate only; no public release publication, compatibility certification broadening, SDK stability, all-saver migration, Manager install mutation, Workbench MVP, platform expansion, or AIDE runtime authority is admitted.",
    }


def write_reports(report: dict[str, Any]) -> None:
    GATE_REPORT.parent.mkdir(parents=True, exist_ok=True)
    GATE_REPORT.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Publication-Prep Gate Report",
        "",
        f"Status: {report['status']}",
        f"Candidate: {report['candidate_id']}",
        f"Recommended state: {report['recommended_state']}",
        "",
        "## Checks",
        "",
    ]
    for item in report.get("checks", []):
        lines.append(f"- {item.get('id')}: {item.get('status')} - {item.get('message')}")
    lines.extend([
        "",
        "## Boundary",
        "",
        report["claim_boundary"],
        "",
    ])
    GATE_REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    report = build_report()
    write_reports(report)
    if report["status"] != "publication-ready":
        for item in report.get("checks", []):
            if item.get("status") != "pass":
                print(f"{item.get('id')}: {item.get('message')}", file=sys.stderr)
        print("Plasma v2 publication-prep gate hold", file=sys.stderr)
        return 1
    print("Plasma v2 publication-prep gate publication-ready")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
