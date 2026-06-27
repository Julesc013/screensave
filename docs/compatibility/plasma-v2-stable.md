# Plasma v2 Stable-Promotion Compatibility Notes

Status: stable-promotion compatibility wording for `plasma-v2-rc1`.

This document records evidence-classed support facts for the stable-promotion
review. It is not an operating-system certification and it is not publication.

Rules:

- Current Windows x86 `.scr` artifact evidence can support a stable-promotion
  decision only inside the named `plasma-v2-rc1` scope.
- PE audit zero violations record binary facts and do not imply OS
  certification.
- The VS2017 `v141_xp` saver lane supports the preservation-oriented artifact
  profile, but does not certify Windows 95, Windows 98, Windows ME, or NT4.
- GL11 remains the first optional acceleration candidate and does not imply all
  accelerated paths.
- `packc` v1-candidate remains data-only and does not imply an executable
  plugin ecosystem.
- Manager preview does not imply install mutation.
- Workbench release-readiness does not imply publication.
- The current stable-promotion review is held until final human artistic acceptance is supplied.

Only evidence explicitly classified as `certified` may be worded as ordinary
public compatibility coverage.
