"""Validate ScreenSave AIDE vendor lock governance."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
VENDOR_LOCK = ROOT / ".aide" / "vendor-lock.yaml"
AIDE_LOCK = ROOT / ".aide" / "aide_lite.lock.toml"
MANIFEST = ROOT / ".aide" / "distribution" / "screensave-aide-lite-manifest.json"

EXPECTED_HEAD = "20deb74a59cbf37aed99a95c11dbf6f750430146"
EXPECTED_PIN = "492faa4f1a8280ba67954aa4fc252e79f2e19c15"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [VENDOR_LOCK, AIDE_LOCK, MANIFEST]:
        require(path.exists(), f"Missing AIDE vendor governance path: {path.relative_to(ROOT)}", errors)

    vendor_text = VENDOR_LOCK.read_text(encoding="utf-8") if VENDOR_LOCK.exists() else ""
    for needle in [
        "schema_version: screensave.aide.vendor-lock.v0",
        f"pinned_commit: {EXPECTED_PIN}",
        f"latest_observed_head: {EXPECTED_HEAD}",
        "distribution_manifest_v1_status: accepted-upstream",
        "project_lock_v0_status: accepted-upstream",
        "ownership_ledger_v1_status: accepted-upstream",
        "local_process_host_fixture_status: accepted-upstream-fixture-only",
        "general_worker_admitted: false",
        "preview_apply_rollback_admitted: false",
        "automatic_merge_allowed: false",
        "automatic_release_allowed: false",
        "ScreenSave product truth",
    ]:
        require(needle in vendor_text, f"vendor-lock.yaml missing {needle!r}", errors)

    if AIDE_LOCK.exists():
        with AIDE_LOCK.open("rb") as handle:
            lock = tomllib.load(handle)
        require(lock.get("source", {}).get("pinned_commit") == EXPECTED_PIN, "AIDE lock pinned commit must remain unchanged.", errors)
        require(lock.get("source", {}).get("latest_observed_head") == EXPECTED_HEAD, "AIDE lock latest observed head mismatch.", errors)
        require(lock.get("distribution_manifest_v1", {}).get("upstream_status") == "accepted-upstream", "DistributionManifest upstream status mismatch.", errors)
        require(lock.get("project_lock_v0", {}).get("automatic_update_allowed") is False, "ProjectLock automatic update must stay false.", errors)
        require(lock.get("ownership_ledger_v1", {}).get("automatic_update_allowed") is False, "OwnershipLedger automatic update must stay false.", errors)
        fixture = lock.get("local_process_host_fixture", {})
        require(fixture.get("general_worker_admitted") is False, "General worker must remain blocked.", errors)
        require(fixture.get("preview_apply_rollback_admitted") is False, "Preview/apply/rollback must remain blocked.", errors)
        require(fixture.get("automatic_release_allowed") is False, "Automatic release must remain blocked.", errors)

    if MANIFEST.exists():
        manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
        require(manifest.get("latest_observed_head") == EXPECTED_HEAD, "Distribution manifest latest head mismatch.", errors)
        require(manifest.get("source_commit") == EXPECTED_PIN, "Distribution manifest source commit mismatch.", errors)
        for key in [
            "general_worker_admitted",
            "preview_apply_rollback_admitted",
            "automatic_merge_allowed",
            "automatic_release_allowed",
        ]:
            require(manifest.get(key) is False, f"Distribution manifest must keep {key} false.", errors)
        require("compatibility" in manifest.get("claim_boundary", ""), "Distribution manifest must preserve compatibility boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("AIDE vendor lock checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
