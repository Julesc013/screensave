# Plasma v2 Stable Candidate

Status: stable-candidate, not stable.

Recorded on: 2026-06-27.

Gate: `tools/scripts/check_plasma_v2_stable_candidate.py`.

## Evidence

- Software reference proof remains the oracle for `plasma.v2.reference.preview`.
- GL11 is admitted only as the first acceleration candidate and records
  software-reference fallback in the portable proof runner.
- The acceleration matrix lives at
  `validation/captures/plasma-v2/acceleration/matrix.json`.
- The performance/resource envelope lives at
  `validation/captures/plasma-v2/performance/envelope.json`.
- Packc is recorded as a bounded data-only v1 candidate.
- Workbench can Author, Inspect, Compare, Profile, and Review the Plasma v2
  candidate without becoming a duplicate proof runner.
- AIDE indexes and exports evidence references only.

## Boundary

Stable candidate is not stable release. It is not final artistic acceptance,
compatibility certification, public SDK stability, all-saver migration,
Manager packaging, or executable pack admission.

The next track is `plasma-v2-release-readiness`.
