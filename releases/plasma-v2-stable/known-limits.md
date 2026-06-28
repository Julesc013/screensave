# Plasma v2 Stable Known Limits

This packet prepares Plasma v2 stable publication. It does not publish a release,
broaden compatibility certification, freeze a public SDK, migrate all Core
savers, or admit a Manager install/apply path.

Known limits:

- Scope is `plasma-v2-rc1` and profile `plasma.v2.reference.preview`.
- Evidence is current Windows x86 `.scr` artifact evidence, not broad OS
  preservation certification.
- The software/reference path is canonical.
- GDI remains mandatory and GL11 remains optional.
- GL21, GL33, GL46, D3D, Vulkan, Metal, macOS, Linux, public SDK, marketplace,
  and executable plugin lanes remain outside this packet.
- Packc remains data-only v1-candidate; packs must not carry scripts, DLLs,
  macros, network references, or runtime executable plugins.
- Manager evidence remains preview/inspection only and does not mutate
  installation.
- Workbench evidence remains release-readiness inspection only and does not
  create the graphical Workbench MVP.
- Compatibility certification is not broadened for Windows 95, Windows 98,
  Windows ME, NT4, or any broad OS family.
