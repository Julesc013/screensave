# Plasma v2 Stable Install Notes

This PAW-J packet does not perform installation, create an installer, publish a
download, or admit Manager install/apply mutation.

Operator handling:

1. Treat this directory as a local publication-prep packet until PAW-K or a
   later publication gate records a publication receipt.
2. Use the staged artifact reference
   `out/msvc/vs2017_xp/Release/plasma/plasma.scr` for local verification only.
3. Keep existing saver installation flows unchanged.
4. Do not infer compatibility certification from the package, PE audit, support
   matrix, or artifact hash.
5. Do not treat Manager preview evidence as install/apply authorization.
