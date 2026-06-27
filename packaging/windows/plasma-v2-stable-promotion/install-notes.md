# Plasma v2 Stable-Promotion Install Notes

No installer mutation is admitted by this packet. The staged artifact reference is the current Windows x86 `.scr` output built by the fixed ScreenSave build profile.

Operator handling:

1. Treat this packet as internal stable-promotion evidence, not publication.
2. Keep existing saver installation flows unchanged.
3. Do not infer compatibility certification from the package, PE audit, or artifact hash.
4. Use Manager preview only for inspection until a later installation gate admits mutation.
