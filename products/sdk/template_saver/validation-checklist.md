# Template Saver Validation Checklist

Before wiring a copied saver into build targets or status docs:

1. Replace every `template_saver` file name, manifest key, and registry root token.
2. Ensure `manifest.ini` names a real product summary, default preset, default theme, and supported renderer tiers honestly.
3. Ensure preset descriptors in code and preset `.ini` authoring files agree on the intended preset keys.
4. Ensure theme descriptors in code and any exported `.theme.ini` samples agree on the intended theme keys.
5. Run `python tools/scripts/check_sdk_surface.py <path-to-new-saver-root>`.
6. Add or update a lightweight smoke check under `tests/`.
7. Keep BenchLab, `anthology`, and `suite` unchanged unless the new saver is genuinely ready for those surfaces.

If a new saver needs broader shared behavior, prove reuse first.
Do not promote product-local logic into `platform/` because one saver wants it early.
