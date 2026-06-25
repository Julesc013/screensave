"""Generate deterministic ScreenSave catalog inventory files."""

from __future__ import annotations

import argparse
import difflib
import json
import pathlib
import sys
import tempfile
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PRODUCTS_PATH = ROOT / "catalog" / "products.toml"
PROFILES_PATH = ROOT / "catalog" / "artifact_profiles.toml"
GENERATED_DIR = ROOT / "catalog" / "generated"


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def product_root(kind: str, key: str) -> pathlib.Path:
    if kind == "saver":
        return ROOT / "products" / "savers" / key
    if kind == "app":
        return ROOT / "products" / "apps" / key
    if kind == "sdk":
        return ROOT / "products" / "sdk"
    raise ValueError(f"unsupported product kind: {kind}")


def sorted_files(root: pathlib.Path, relative: str, pattern: str) -> list[str]:
    folder = root / relative
    if not folder.exists():
        return []
    return [repo_path(path) for path in sorted(folder.glob(pattern), key=lambda item: item.name)]


def default_section(products_data: dict[str, Any], kind: str) -> dict[str, Any]:
    defaults = products_data.get("defaults", {})
    section = defaults.get(kind, {})
    if isinstance(section, dict):
        return section
    return {}


def default_routing(products_data: dict[str, Any]) -> dict[str, Any]:
    defaults = products_data.get("defaults", {})
    routing = defaults.get("routing", {})
    if isinstance(routing, dict):
        return routing
    return {}


def resolve_product(products_data: dict[str, Any], product: dict[str, Any]) -> dict[str, Any]:
    kind = str(product["kind"])
    key = str(product["key"])
    root = product_root(kind, key)
    defaults = default_section(products_data, kind)
    routing_defaults = default_routing(products_data)

    def value(name: str, fallback: Any = None) -> Any:
        if name in product:
            return product[name]
        if name in defaults:
            return defaults[name]
        return fallback

    inventory: dict[str, Any] = {
        "key": key,
        "name": product["name"],
        "kind": kind,
        "family": product.get("family", ""),
        "role": product.get("role", ""),
        "root": repo_path(root),
        "release_channel": value("release_channel", ""),
        "maturity": value("maturity", ""),
        "compatibility_status": value("compatibility_status", ""),
        "artifact_profiles": value("artifact_profiles", []),
        "source_units": value("source_units", []),
        "required_presets": value("required_presets", []),
        "reference_path_required": bool(value("reference_path_required", False)),
        "semantic_contract": value("semantic_contract", ""),
        "routing": {
            "minimum_kind": product.get("minimum_kind", routing_defaults.get("minimum_kind", "")),
            "preferred_kind": product.get("preferred_kind", routing_defaults.get("preferred_kind", "")),
            "quality_class": product.get("quality_class", routing_defaults.get("quality_class", "")),
        },
        "files": {
            "sources": sorted_files(root, "src", "*.c"),
            "resources": sorted_files(root, "src", "*.rc"),
            "headers": sorted_files(root, "src", "*.h"),
            "presets": sorted_files(root, "presets", "*.ini"),
            "tests": sorted_files(root, "tests", "*"),
        },
    }
    return inventory


def build_inventory() -> dict[str, Any]:
    products_data = load_toml(PRODUCTS_PATH)
    profiles_data = load_toml(PROFILES_PATH)
    products = [resolve_product(products_data, product) for product in products_data["products"]]
    profiles = profiles_data.get("artifact_profiles", [])

    return {
        "schema_version": 1,
        "generated_by": "tools/cataloggen/cataloggen.py",
        "sources": [repo_path(PRODUCTS_PATH), repo_path(PROFILES_PATH)],
        "product_count": len(products),
        "saver_count": sum(1 for product in products if product["kind"] == "saver"),
        "artifact_profile_count": len(profiles),
        "products": products,
        "artifact_profiles": [
            {
                "key": profile["key"],
                "name": profile["name"],
                "product_classes": profile["product_classes"],
                "host": profile["host"],
                "artifact_extension": profile["artifact_extension"],
                "machine": profile["machine"],
                "evidence_status": profile["evidence_status"],
                "public_claim": profile["public_claim"],
            }
            for profile in profiles
        ],
    }


def render_json(inventory: dict[str, Any]) -> str:
    return json.dumps(inventory, indent=2, sort_keys=True) + "\n"


def render_makefile(inventory: dict[str, Any]) -> str:
    def assignment(name: str, values: list[str]) -> str:
        if values:
            return f"{name} := " + " ".join(values)
        return f"{name} :="

    lines = [
        "# Generated by tools/cataloggen/cataloggen.py; do not edit by hand.",
        "SCREENSAVE_CATALOG_SCHEMA := 1",
        "SCREENSAVE_PRODUCTS := " + " ".join(product["key"] for product in inventory["products"]),
        "SCREENSAVE_SAVERS := " + " ".join(product["key"] for product in inventory["products"] if product["kind"] == "saver"),
        "SCREENSAVE_APPS := " + " ".join(product["key"] for product in inventory["products"] if product["kind"] == "app"),
        "",
    ]
    for index, product in enumerate(inventory["products"]):
        macro_key = product["key"].upper().replace("-", "_")
        lines.append(assignment(f"SCREENSAVE_{macro_key}_SOURCES", product["files"]["sources"]))
        lines.append(assignment(f"SCREENSAVE_{macro_key}_RESOURCES", product["files"]["resources"]))
        if index + 1 < len(inventory["products"]):
            lines.append("")
    return "\n".join(lines) + "\n"


def render_markdown(inventory: dict[str, Any]) -> str:
    lines = [
        "# Generated Product Inventory",
        "",
        "Generated by `tools/cataloggen/cataloggen.py`; do not edit by hand.",
        "",
        f"- Products: {inventory['product_count']}",
        f"- Savers: {inventory['saver_count']}",
        f"- Artifact profiles: {inventory['artifact_profile_count']}",
        "",
        "| Key | Kind | Channel | Maturity | Profiles | Sources | Presets |",
        "| --- | --- | --- | --- | --- | ---: | ---: |",
    ]
    for product in inventory["products"]:
        lines.append(
            "| {key} | {kind} | {channel} | {maturity} | {profiles} | {sources} | {presets} |".format(
                key=product["key"],
                kind=product["kind"],
                channel=product["release_channel"],
                maturity=product["maturity"],
                profiles=", ".join(product["artifact_profiles"]),
                sources=len(product["files"]["sources"]),
                presets=len(product["files"]["presets"]),
            )
        )
    return "\n".join(lines) + "\n"


def output_map(inventory: dict[str, Any]) -> dict[str, str]:
    return {
        "products_inventory.json": render_json(inventory),
        "products_sources.mk": render_makefile(inventory),
        "products_table.md": render_markdown(inventory),
    }


def write_outputs(base: pathlib.Path, outputs: dict[str, str]) -> None:
    base.mkdir(parents=True, exist_ok=True)
    for name, text in outputs.items():
        (base / name).write_text(text, encoding="utf-8", newline="\n")


def check_outputs(outputs: dict[str, str]) -> list[str]:
    errors: list[str] = []
    for name, expected in outputs.items():
        path = GENERATED_DIR / name
        if not path.exists():
            errors.append(f"missing generated output: {repo_path(path)}")
            continue
        actual = path.read_text(encoding="utf-8")
        if actual != expected:
            diff = "\n".join(
                difflib.unified_diff(
                    actual.splitlines(),
                    expected.splitlines(),
                    fromfile=repo_path(path),
                    tofile=f"expected/{name}",
                    lineterm="",
                )
            )
            errors.append(f"stale generated output: {repo_path(path)}\n{diff}")
    return errors


def command_generate(_args: argparse.Namespace) -> int:
    write_outputs(GENERATED_DIR, output_map(build_inventory()))
    return 0


def command_check(_args: argparse.Namespace) -> int:
    outputs = output_map(build_inventory())
    errors = check_outputs(outputs)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        with tempfile.TemporaryDirectory(prefix="screensave-cataloggen-") as tmp:
            write_outputs(pathlib.Path(tmp), outputs)
        return 1
    print("Catalog generated inventory is current.")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    generate = subparsers.add_parser("generate", help="write catalog/generated outputs")
    generate.set_defaults(func=command_generate)

    check = subparsers.add_parser("check", help="verify catalog/generated outputs")
    check.set_defaults(func=command_check)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
