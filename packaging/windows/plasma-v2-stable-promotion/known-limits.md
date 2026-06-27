# Plasma v2 Stable-Promotion Known Limits

This staging packet is for the Plasma v2 stable-promotion decision only. It is not publication, compatibility certification, installer admission, public SDK stability, or a promise that every future Plasma lane is stable.

Known limits:

- Evidence is scoped to `plasma-v2-rc1` and the current Windows x86 `.scr` artifact path.
- The baseline delivery lane remains VS2017 `v141_xp`, Win32 x86, ANSI Win32 host, mandatory GDI, and optional GL11.
- GL11 remains an optional accelerated candidate, not a requirement and not a claim for newer GL tiers.
- Premium, glyph, ribbon, heightfield, D3D, Vulkan, Metal, macOS, Linux, public SDK, marketplace, and executable plugin lanes remain outside this packet.
- Compatibility certification is not claimed for Windows 95, Windows 98, Windows ME, NT4, or any broad OS family.
