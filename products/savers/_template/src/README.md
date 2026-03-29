# Template Saver Source

Purpose: real source skeleton for a new saver product cloned from the repository template.
Belongs here: template-local module, config, preset, theme, simulation, render, resource, and version files that contributors copy and rename into a new saver.
Does not belong here: shared platform code, build-lane outputs, or a second shipping product.
Current stage: `C10` makes this a compile-oriented starter surface rather than a placeholder.
Type: product template source.

## File Roles

- `template_saver_entry.c`: real `.scr` entry shim pattern using `screensave_scr_main(...)`
- `template_saver_module.c`: saver identity, capability flags, config hooks, and callbacks
- `template_saver_config.c`: defaults, registry persistence, import-export hooks, randomization, and config dialog pattern
- `template_saver_presets.c`: starter preset descriptors and product-local preset values
- `template_saver_themes.c`: starter theme descriptors
- `template_saver_sim.c`: bounded session lifecycle and state stepping
- `template_saver_render.c`: conservative renderer-agnostic draw path using shared renderer primitives
- `template_saver_config.rc` and `template_saver_resource.h`: example Win32 settings dialog resources
- `template_saver_version.rc`: product-owned version metadata template
