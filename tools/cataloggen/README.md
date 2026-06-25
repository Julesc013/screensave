# Catalog Generator

`cataloggen.py` turns the reviewed catalog TOML files into committed,
deterministic inventory outputs under `catalog/generated/`.

The generator is intentionally small and stdlib-only. It does not build
ScreenSave, publish artifacts, or redefine product semantics. It gives build,
packaging, Manager, Workbench, and proof tooling a shared generated inventory to
consume without making Python mandatory for legacy saver builds.

Use:

```text
python tools/cataloggen/cataloggen.py generate
python tools/cataloggen/cataloggen.py check
```

Type: repository control tool.
