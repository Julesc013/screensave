"""Validate ScreenSave AIDE ownership ledger boundaries."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "ownership" / "ownership-ledger.toml"
EXPECTED_PATHS = {
    ".aide/scripts",
    ".aide/policies",
    ".aide/work_units",
    ".aide/evidence",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    require(LEDGER.exists(), f"Missing ownership ledger: {LEDGER.relative_to(ROOT)}", errors)
    if LEDGER.exists():
        with LEDGER.open("rb") as handle:
            ledger = tomllib.load(handle)
        require(ledger.get("schema_version") == 1, "ownership ledger schema_version must be 1.", errors)
        require(ledger.get("status") == "active", "ownership ledger status must be active.", errors)
        require(ledger.get("latest_observed_head") == "20deb74a59cbf37aed99a95c11dbf6f750430146", "ownership ledger latest observed head mismatch.", errors)
        paths = {item.get("path") for item in ledger.get("owned_paths", [])}
        require(EXPECTED_PATHS <= paths, "ownership ledger missing required imported/local paths.", errors)
        policy = ledger.get("policy", {})
        require(policy.get("source_mutation_by_aide") is False, "AIDE source mutation must remain false.", errors)
        require(policy.get("automatic_merge") is False, "automatic merge must remain false.", errors)
        require(policy.get("automatic_release") is False, "automatic release must remain false.", errors)
        require(policy.get("compatibility_certification") is False, "compatibility certification must remain false.", errors)
        require(policy.get("fixture_worker_only") is True, "local process host must remain fixture-only.", errors)
        require("does not authorize source mutation" in ledger.get("claim_boundary", ""), "ownership ledger must preserve source mutation boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("AIDE ownership checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
