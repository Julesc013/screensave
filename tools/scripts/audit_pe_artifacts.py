"""Audit PE architecture, subsystem, and import facts for ScreenSave artifacts."""

from __future__ import annotations

import argparse
import pathlib
import struct
import sys
from dataclasses import dataclass


ROOT = pathlib.Path(__file__).resolve().parents[2]

MACHINES = {
    0x014C: "x86",
    0x8664: "x64",
}

OPTIONAL_MAGIC = {
    0x010B: "PE32",
    0x020B: "PE32+",
}

DEFAULT_ROOTS = [
    ROOT / "out" / "portable" / "screensave-core-c16-baseline",
    ROOT / "out" / "installer" / "screensave-installer-c16-baseline",
    ROOT / "out" / "msvc" / "vs2022" / "Release",
    ROOT / "out" / "mingw" / "i686" / "debug",
]


@dataclass
class Section:
    name: str
    virtual_address: int
    virtual_size: int
    raw_pointer: int
    raw_size: int


@dataclass
class PeFacts:
    path: pathlib.Path
    machine: str
    optional_format: str
    subsystem: int
    subsystem_version: str
    imports: list[str]
    flags: list[str]


def read_c_string(data: bytes, offset: int) -> str:
    end = data.find(b"\x00", offset)
    if end < 0:
        end = len(data)
    return data[offset:end].decode("ascii", errors="replace")


def rva_to_offset(rva: int, sections: list[Section]) -> int | None:
    for section in sections:
        size = max(section.virtual_size, section.raw_size)
        if section.virtual_address <= rva < section.virtual_address + size:
            return section.raw_pointer + (rva - section.virtual_address)
    return None


def parse_imports(data: bytes, sections: list[Section], import_rva: int, import_size: int) -> list[str]:
    if import_rva == 0 or import_size == 0:
        return []
    offset = rva_to_offset(import_rva, sections)
    if offset is None:
        return ["<unmapped-import-table>"]

    imports: list[str] = []
    descriptor_size = 20
    while offset + descriptor_size <= len(data):
        original_first_thunk, _time_date_stamp, _forwarder_chain, name_rva, first_thunk = struct.unpack_from(
            "<IIIII", data, offset
        )
        if original_first_thunk == 0 and name_rva == 0 and first_thunk == 0:
            break
        name_offset = rva_to_offset(name_rva, sections)
        if name_offset is None:
            imports.append("<unmapped-import-name>")
        else:
            imports.append(read_c_string(data, name_offset))
        offset += descriptor_size
    return imports


def audit_pe(path: pathlib.Path) -> PeFacts:
    data = path.read_bytes()
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError("not an MZ executable")

    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset : pe_offset + 4] != b"PE\x00\x00":
        raise ValueError("missing PE signature")

    coff_offset = pe_offset + 4
    machine_value, section_count, _timestamp, _symbols, _symbol_count, optional_size, _characteristics = struct.unpack_from(
        "<HHIIIHH", data, coff_offset
    )
    optional_offset = coff_offset + 20
    magic = struct.unpack_from("<H", data, optional_offset)[0]
    if magic not in OPTIONAL_MAGIC:
        raise ValueError(f"unknown optional header magic 0x{magic:04x}")

    if magic == 0x010B:
        subsystem_offset = optional_offset + 68
        data_directory_offset = optional_offset + 96
    else:
        subsystem_offset = optional_offset + 68
        data_directory_offset = optional_offset + 112

    major_subsystem, minor_subsystem = struct.unpack_from("<HH", data, optional_offset + 48)
    subsystem = struct.unpack_from("<H", data, subsystem_offset)[0]
    import_rva, import_size = struct.unpack_from("<II", data, data_directory_offset + 8)

    sections: list[Section] = []
    section_offset = optional_offset + optional_size
    for index in range(section_count):
        base = section_offset + index * 40
        raw_name = data[base : base + 8].split(b"\x00", 1)[0]
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from("<IIII", data, base + 8)
        sections.append(
            Section(
                name=raw_name.decode("ascii", errors="replace"),
                virtual_address=virtual_address,
                virtual_size=virtual_size,
                raw_pointer=raw_pointer,
                raw_size=raw_size,
            )
        )

    imports = parse_imports(data, sections, import_rva, import_size)
    flags: list[str] = []
    machine = MACHINES.get(machine_value, f"unknown-0x{machine_value:04x}")
    optional_format = OPTIONAL_MAGIC[magic]
    subsystem_version = f"{major_subsystem}.{minor_subsystem}"

    lower_imports = [item.lower() for item in imports]
    if machine != "x86" and "mingw\\i686" in str(path).lower():
        flags.append("VIOLATION: mingw/i686 artifact is not x86")
    if optional_format != "PE32" and path.suffix.lower() == ".scr":
        flags.append("VIOLATION: .scr artifact is not PE32")
    if any(name.startswith("vcruntime") or name.startswith("ucrtbase") or name.startswith("api-ms-win-crt") for name in lower_imports):
        flags.append("NOTE: dynamic VC/UCRT dependency present")
    if subsystem_version not in {"4.0", "5.1"}:
        flags.append("NOTE: subsystem is not a preservation-floor value")

    return PeFacts(
        path=path,
        machine=machine,
        optional_format=optional_format,
        subsystem=subsystem,
        subsystem_version=subsystem_version,
        imports=sorted(imports, key=str.lower),
        flags=flags,
    )


def iter_artifacts(roots: list[pathlib.Path]) -> list[pathlib.Path]:
    paths: list[pathlib.Path] = []
    for root in roots:
        if not root.exists():
            continue
        for suffix in ("*.scr", "*.exe"):
            paths.extend(root.rglob(suffix))
    return sorted(paths)


def format_report(facts: list[PeFacts]) -> str:
    lines = [
        "ScreenSave PE Artifact Audit",
        "===========================",
        "",
        "This report records binary facts. Notes and violations are evidence, not release approval.",
        "",
    ]
    for item in facts:
        rel = item.path.relative_to(ROOT)
        lines.append(f"Artifact: {rel}")
        lines.append(f"  machine: {item.machine}")
        lines.append(f"  format: {item.optional_format}")
        lines.append(f"  subsystem: {item.subsystem} ({item.subsystem_version})")
        lines.append(f"  imports: {', '.join(item.imports) if item.imports else '<none>'}")
        lines.append(f"  flags: {', '.join(item.flags) if item.flags else 'none'}")
        lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="*", help="Optional artifact roots or files. Defaults to known out/ artifact roots.")
    parser.add_argument("--output", help="Write the report to this path instead of stdout.")
    parser.add_argument("--fail-on-violation", action="store_true", help="Return nonzero when a VIOLATION flag is found.")
    args = parser.parse_args()

    if args.paths:
        roots = [(ROOT / value).resolve() if not pathlib.Path(value).is_absolute() else pathlib.Path(value) for value in args.paths]
        artifacts: list[pathlib.Path] = []
        for path in roots:
            if path.is_dir():
                artifacts.extend(iter_artifacts([path]))
            else:
                artifacts.append(path)
    else:
        artifacts = iter_artifacts(DEFAULT_ROOTS)

    facts: list[PeFacts] = []
    errors: list[str] = []
    for path in artifacts:
        try:
            facts.append(audit_pe(path))
        except Exception as exc:  # noqa: BLE001 - reports binary parse failures as audit facts.
            errors.append(f"{path}: {exc}")

    report = format_report(facts)
    if errors:
        report += "\nParse errors:\n" + "\n".join(f"  {error}" for error in errors) + "\n"

    if args.output:
        output_path = (ROOT / args.output).resolve() if not pathlib.Path(args.output).is_absolute() else pathlib.Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(report, encoding="utf-8")
    else:
        print(report)

    if args.fail_on_violation and any(any(flag.startswith("VIOLATION:") for flag in item.flags) for item in facts):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
