"""Validate the isolated Plasma V3 visual-core spike."""

from __future__ import annotations

import json
import pathlib
import re
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v3"
DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v3-visual-core-reset.md"
CAPTURE = ROOT / "tools" / "scripts" / "plasma_v3_capture.py"
OUTPUT = ROOT / "validation" / "captures" / "plasma-v3" / "visual-spike"

REQUIRED_FILES = [
    DOC,
    SRC / "plasma_v3_types.h",
    SRC / "plasma_v3_spec.h",
    SRC / "plasma_v3_spec.c",
    SRC / "plasma_v3_plan.h",
    SRC / "plasma_v3_plan.c",
    SRC / "plasma_v3_runtime.h",
    SRC / "plasma_v3_runtime.c",
    SRC / "field" / "plasma_v3_field.c",
    SRC / "material" / "plasma_v3_material.c",
    SRC / "treatment" / "plasma_v3_treatment.c",
    SRC / "present" / "plasma_v3_present.c",
    SRC / "proof" / "plasma_v3_proof.c",
    ROOT / "products" / "savers" / "plasma" / "tests" / "plasma_v3_visual_smoke.c",
    CAPTURE,
]

REQUIRED_TEXT = {
    DOC: [
        "Plasma V3 is a visual-core reset",
        "not a publication lane",
        "direct controls first",
        "software/reference path is canonical",
    ],
    SRC / "plasma_v3_types.h": [
        "screensave.plasma.v3.visual-core-spike",
        "PLASMA_V3_CONTROL_FIELD_SCALE",
        "plasma_v3_runtime_buffers",
    ],
    SRC / "plasma_v3_runtime.c": [
        "plasma_v3_field_render",
        "plasma_v3_material_map",
        "plasma_v3_treatment_apply",
        "plasma_v3_present_flat",
    ],
    CAPTURE: [
        "control-influence.json",
        "review-contact-sheet.ppm",
        "not publication",
    ],
}

FORBIDDEN_PATTERNS = [
    (re.compile(r"windows\.h", re.I), "Win32 header"),
    (re.compile(r"\bHWND\b"), "Win32 HWND"),
    (re.compile(r"\bHDC\b"), "Win32 HDC"),
    (re.compile(r"\bHINSTANCE\b"), "Win32 HINSTANCE"),
    (re.compile(r"\bDWORD\b"), "Win32 DWORD"),
    (re.compile(r"\bmalloc\b"), "per-frame allocation risk"),
    (re.compile(r"\bfree\s*\("), "manual free"),
    (re.compile(r"_Static_assert"), "post-C89 static assert"),
    (re.compile(r"stdbool\.h"), "C99 bool header"),
    (re.compile(r"stdint\.h"), "C99 int header"),
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def scan_required(errors: list[str]) -> None:
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing Plasma V3 visual spike file: {repo_path(path)}", errors)
    if errors:
        return
    for path, needles in REQUIRED_TEXT.items():
        text = path.read_text(encoding="ascii")
        for needle in needles:
            require(needle in text, f"{repo_path(path)} missing {needle!r}", errors)
    for path in SRC.rglob("*.[ch]"):
        text = path.read_text(encoding="ascii")
        for pattern, label in FORBIDDEN_PATTERNS:
            if pattern.search(text):
                errors.append(f"{repo_path(path)} contains forbidden {label}")


def run_capture(errors: list[str]) -> None:
    result = subprocess.run(
        [sys.executable, str(CAPTURE), "--output-dir", str(OUTPUT), "--width", "192", "--height", "108"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if result.returncode != 0:
        errors.append("Plasma V3 capture check failed:\n" + result.stdout + result.stderr)
        return
    summary_path = OUTPUT / "proof-summary.json"
    require(summary_path.exists(), f"Missing {repo_path(summary_path)}", errors)
    if not summary_path.exists():
        return
    summary = json.loads(summary_path.read_text(encoding="ascii"))
    require(summary.get("status") == "pass", "Plasma V3 proof summary must pass.", errors)
    require(summary.get("claim_boundary", "").startswith("deterministic local"), "Claim boundary must remain local.", errors)
    rows = summary.get("control_influence", [])
    require(len(rows) >= 4, "Control influence matrix must include direct controls.", errors)
    for row in rows:
        require(row.get("changed") is True, f"Control {row.get('control')} must change frame output.", errors)
    for rel_path in [
        summary.get("contact_sheet"),
        summary.get("contact_sheet_markdown"),
        summary.get("frame_time_log"),
        "validation/captures/plasma-v3/visual-spike/control-influence.json",
    ]:
        require(bool(rel_path) and (ROOT / str(rel_path)).exists(), f"Missing proof output {rel_path}", errors)


def main() -> int:
    errors: list[str] = []
    scan_required(errors)
    if not errors:
        run_capture(errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma V3 visual spike checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
