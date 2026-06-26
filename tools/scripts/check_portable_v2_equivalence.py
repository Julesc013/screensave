"""Validate exact portable v2 equivalence for the admitted canary profiles."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
EQUIVALENCE = ROOT / "tools" / "sslab" / "sslab_equivalence.py"
OUTPUT = ROOT / "out" / "checks" / "portable-v2-equivalence" / "equivalence.json"
WORK_ROOT = ROOT / "out" / "checks" / "portable-v2-equivalence" / "captures"

EXPECTED_HASHES = {
    "nocturne.reference.v0": {
        8: "5394a14b6622c17bfb10cd5721c08a4c92cdbddfb12f55c954ef1d5f6ef878b2",
    },
    "ricochet.reference.v1": {
        0: "b8c7c0ea922be06b559485e22a16c286fd29e7496437cfb017a74aec68c6235a",
        4: "7a1615e04d27e9dcb58da30b9520fcfa8dfe4b8fd5c14199750ef95754206ff1",
        8: "a50582e8e8f5517eb2b56e7e01d5b0493b8432375a7e577b152801a4f6574b42",
        32: "65c37015993c76632ce9e384db18b7cfa8c735049c66a72ed2833bbb45f70d25",
    },
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    require(EQUIVALENCE.exists(), "Missing portable v2 equivalence runner.", errors)
    if not errors:
        result = subprocess.run(
            [
                sys.executable,
                str(EQUIVALENCE),
                "--output",
                str(OUTPUT),
                "--work-root",
                str(WORK_ROOT),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        if result.returncode != 0:
            errors.append(f"portable v2 equivalence runner failed: {result.stdout.strip()} {result.stderr.strip()}".strip())

    if not errors:
        receipt = json.loads(OUTPUT.read_text(encoding="utf-8"))
        require(receipt.get("status") == "pass", "Portable v2 equivalence receipt must pass.", errors)
        require(
            receipt.get("claim_boundary")
            == "v1/v2 deterministic equivalence for Nocturne and Ricochet canary proof profiles only",
            "Portable v2 equivalence claim boundary must stay narrow.",
            errors,
        )
        profiles = {item.get("profile"): item for item in receipt.get("profiles", [])}
        require(set(profiles) == set(EXPECTED_HASHES), "Portable v2 equivalence must cover the admitted canary profiles.", errors)
        for profile_key, frames in EXPECTED_HASHES.items():
            profile = profiles.get(profile_key, {})
            require(profile.get("status") == "pass", f"{profile_key} equivalence must pass.", errors)
            require(profile.get("frame_hash_match") is True, f"{profile_key} frame hashes must match.", errors)
            require(profile.get("lifecycle_match") is True, f"{profile_key} lifecycle class must match.", errors)
            require(profile.get("profile_pass_class_match") is True, f"{profile_key} short-soak class must match.", errors)
            captures = {int(item.get("frame", -1)): item for item in profile.get("captures", [])}
            require(set(captures) == set(frames), f"{profile_key} must emit the expected frame set.", errors)
            for frame, expected_hash in frames.items():
                capture = captures.get(frame, {})
                require(capture.get("match") is True, f"{profile_key} frame {frame} must match across v1/v2.", errors)
                require(capture.get("v1_rgba_sha256") == expected_hash, f"{profile_key} frame {frame} v1 hash changed.", errors)
                require(capture.get("v2_rgba_sha256") == expected_hash, f"{profile_key} frame {frame} v2 hash changed.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Portable v2 equivalence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
