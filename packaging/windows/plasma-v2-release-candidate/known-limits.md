# Plasma v2 Release-Candidate Known Limits

Status: staged release-candidate notes, not release notes.

- The package is not published.
- `stable = false` remains the project state until a later stable-promotion gate.
- Compatibility certification is not claimed.
- The current `.scr` artifact reference is an output path, not a committed binary.
- GL11 is the first acceleration candidate and may fall back to the software/GDI path.
- The package does not admit executable plug-ins or script payloads.
- Manager support is preview and inspection only; it does not mutate installation state.
- Workbench support is release-readiness review only; it does not publish a release.
