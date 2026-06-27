# Plasma v2 Release-Candidate Compatibility Notes

Status: release-candidate compatibility wording.

Plasma v2 release-candidate readiness is not an operating-system certification.
The current package stage records buildable artifact references, hashes, and
support wording for `plasma-v2-rc1`.

Rules:

- Current Windows artifact evidence can support release-candidate readiness.
- PE audit zero violations do not imply OS certification.
- GL11 candidate evidence does not imply all accelerated paths.
- `packc` v1-candidate does not imply an executable plugin ecosystem.
- Manager preview does not imply install mutation.
- Workbench release-readiness does not imply publication.
- `stable = false` remains required until a later stable-promotion gate.

The broad Windows band remains evidence-classed under `specs/compatibility.md`.
Only `certified` evidence should be worded as ordinary public compatibility
coverage.
