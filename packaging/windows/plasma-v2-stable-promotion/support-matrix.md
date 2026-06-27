# Plasma v2 Stable-Promotion Support Matrix

This matrix records evidence classes for the stable-promotion packet. It does not create compatibility certification.

| Lane | Evidence class | Status | Boundary |
| --- | --- | --- | --- |
| Windows x86 `.scr` artifact | Buildable artifact evidence | Included | Artifact-specific, not broad compatibility certification |
| ANSI Win32 host path | Preserved platform invariant | Included | Current host path only |
| GDI presentation | Mandatory baseline | Included | Lower-band delivery remains required |
| GL11 candidate | Optional accelerated candidate | Included as candidate | Failure must fall back honestly |
| Packc Plasma data pack | v1-candidate data-only path | Included | No scripts, DLLs, macros, or executable plugins |
| Manager pack preview | Preview inspection | Included | No install mutation promise |
| Workbench release-readiness | Inspection/review surface | Included | No publication authority |
| Windows 95/98/ME and NT4 | Targeted preservation aspiration | Not certified | Compatibility certification is not claimed |
| macOS/Linux | Out of scope | Not supported | No host support claim |
| GL21/GL33/GL46, D3D, Vulkan, Metal | Out of scope | Not stable | No accelerated-path certification |
| Public SDK | Out of scope | Not frozen | No SDK stability claim |
