"""Generate deterministic proof captures for the Plasma V3 visual-core spike."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import shutil
import subprocess
import sys
from typing import Dict, Iterable, List, Tuple


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v3"
TEST = ROOT / "products" / "savers" / "plasma" / "tests" / "plasma_v3_visual_smoke.c"
BUILD_DIR = ROOT / "out" / "checks" / "plasma-v3-visual-spike"
DEFAULT_OUTPUT = ROOT / "validation" / "captures" / "plasma-v3" / "visual-spike"

SOURCES = [
    TEST,
    SRC / "plasma_v3_spec.c",
    SRC / "plasma_v3_plan.c",
    SRC / "plasma_v3_runtime.c",
    SRC / "field" / "plasma_v3_field.c",
    SRC / "material" / "plasma_v3_material.c",
    SRC / "treatment" / "plasma_v3_treatment.c",
    SRC / "present" / "plasma_v3_present.c",
    SRC / "proof" / "plasma_v3_proof.c",
]

CASES = [
    ("baseline-gdi", "none", 0, "gdi"),
    ("field-scale-high", "field_scale", 920, "gdi"),
    ("warp-high", "warp", 920, "gdi"),
    ("palette-shift-high", "palette_shift", 940, "gdi"),
    ("phosphor-high", "phosphor", 960, "gdi"),
    ("baseline-gl11-preview", "none", 0, "gl11"),
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def find_compiler() -> str:
    for candidate in ("gcc", "clang", "cc"):
        compiler = shutil.which(candidate)
        if compiler:
            return compiler
    raise RuntimeError("No C compiler found for Plasma V3 capture harness.")


def compile_driver() -> pathlib.Path:
    compiler = find_compiler()
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    exe = BUILD_DIR / "plasma_v3_visual_smoke.exe"
    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC),
        "-I",
        str(SRC / "field"),
        "-I",
        str(SRC / "material"),
        "-I",
        str(SRC / "treatment"),
        "-I",
        str(SRC / "present"),
        "-I",
        str(SRC / "proof"),
        *[str(path) for path in SOURCES],
        "-o",
        str(exe),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        raise RuntimeError("Plasma V3 capture driver compile failed:\n" + result.stdout + result.stderr)
    smoke = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if smoke.returncode != 0:
        raise RuntimeError(
            "Plasma V3 visual smoke failed returncode="
            + str(smoke.returncode)
            + "\n"
            + smoke.stdout
            + smoke.stderr
        )
    return exe


def run_capture_case(
    exe: pathlib.Path,
    output_dir: pathlib.Path,
    label: str,
    control: str,
    value: int,
    renderer: str,
    width: int,
    height: int,
) -> Dict[str, object]:
    case_dir = output_dir / label
    if case_dir.exists():
        shutil.rmtree(case_dir)
    case_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(exe),
        "capture",
        str(case_dir),
        label,
        control,
        str(value),
        renderer,
        str(width),
        str(height),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        raise RuntimeError(
            f"Plasma V3 capture failed for {label} returncode={result.returncode}\n"
            + result.stdout
            + result.stderr
        )
    return parse_summary(case_dir / "summary.txt", case_dir)


def parse_summary(path: pathlib.Path, case_dir: pathlib.Path) -> Dict[str, object]:
    data: Dict[str, object] = {"frames": []}
    for line in path.read_text(encoding="ascii").splitlines():
        if line.startswith("frame="):
            parts = dict(part.split("=", 1) for part in line.split())
            frame_path = case_dir / parts["path"]
            data["frames"].append(
                {
                    "frame": int(parts["frame"]),
                    "hash": parts["hash"],
                    "lit_pixels": int(parts["lit"]),
                    "path": repo_path(frame_path),
                    "sha256": hashlib.sha256(frame_path.read_bytes()).hexdigest(),
                }
            )
        elif "=" in line:
            key, value = line.split("=", 1)
            data[key] = value
    return data


def read_ppm(path: pathlib.Path) -> Tuple[int, int, bytes]:
    with path.open("rb") as handle:
        magic = handle.readline().strip()
        if magic != b"P6":
            raise ValueError(f"{path} is not a P6 PPM")
        dims = handle.readline().strip().split()
        width = int(dims[0])
        height = int(dims[1])
        max_value = handle.readline().strip()
        if max_value != b"255":
            raise ValueError(f"{path} has unsupported PPM max value")
        return width, height, handle.read()


def write_ppm(path: pathlib.Path, width: int, height: int, pixels: bytes) -> None:
    with path.open("wb") as handle:
        handle.write(f"P6\n{width} {height}\n255\n".encode("ascii"))
        handle.write(pixels)


def make_contact_sheet(output_dir: pathlib.Path, cases: List[Dict[str, object]]) -> pathlib.Path:
    selected_paths: List[pathlib.Path] = []
    for case in cases:
        frames = case["frames"]
        selected = next(frame for frame in frames if frame["frame"] == 32)
        selected_paths.append(ROOT / str(selected["path"]))

    images = [read_ppm(path) for path in selected_paths]
    tile_w, tile_h, _ = images[0]
    cols = 3
    rows = (len(images) + cols - 1) // cols
    sheet = bytearray(tile_w * cols * tile_h * rows * 3)
    for idx, (_width, _height, pixels) in enumerate(images):
        col = idx % cols
        row = idx // cols
        for y in range(tile_h):
            src = y * tile_w * 3
            dst = ((row * tile_h + y) * tile_w * cols + col * tile_w) * 3
            sheet[dst : dst + tile_w * 3] = pixels[src : src + tile_w * 3]

    path = output_dir / "review-contact-sheet.ppm"
    write_ppm(path, tile_w * cols, tile_h * rows, bytes(sheet))
    return path


def frame_hash(case: Dict[str, object], frame_index: int) -> str:
    for frame in case["frames"]:
        if frame["frame"] == frame_index:
            return str(frame["hash"])
    raise KeyError(frame_index)


def build_influence(cases: List[Dict[str, object]]) -> List[Dict[str, object]]:
    baseline = cases[0]
    baseline_hash = frame_hash(baseline, 32)
    rows: List[Dict[str, object]] = []
    for case in cases[1:]:
        if case["control"] == "none":
            continue
        current_hash = frame_hash(case, 32)
        rows.append(
            {
                "control": case["control"],
                "value": int(str(case["control_value"])),
                "frame": 32,
                "baseline_hash": baseline_hash,
                "variant_hash": current_hash,
                "changed": current_hash != baseline_hash,
            }
        )
    return rows


def write_frame_time_log(output_dir: pathlib.Path, cases: Iterable[Dict[str, object]]) -> pathlib.Path:
    path = output_dir / "frame-time-log.csv"
    lines = ["case,frame,delta_millis\n"]
    for case in cases:
        for frame in case["frames"]:
            lines.append(f"{case['label']},{frame['frame']},33\n")
    path.write_text("".join(lines), encoding="ascii")
    return path


def write_markdown(output_dir: pathlib.Path, contact_sheet: pathlib.Path, cases: List[Dict[str, object]]) -> pathlib.Path:
    lines = [
        "# Plasma V3 Visual Spike Contact Sheet\n",
        "\n",
        "Status: local deterministic visual-core spike evidence only. This is not publication, compatibility certification, or Plasma v2 promotion evidence.\n",
        "\n",
        f"- Contact sheet: `{repo_path(contact_sheet)}`\n",
        "- Frames shown: frame 32 from each case.\n",
        "- Renderer note: `gl11` is a requested preview route over the same software/reference buffer in this spike.\n",
        "\n",
        "## Cases\n",
        "\n",
    ]
    for case in cases:
        lines.append(
            f"- `{case['label']}` control=`{case['control']}` value=`{case['control_value']}` "
            f"requested=`{case['requested_renderer']}` active=`{case['active_renderer']}`\n"
        )
    path = output_dir / "review-contact-sheet.md"
    path.write_text("".join(lines), encoding="ascii")
    return path


def generate(output_dir: pathlib.Path, width: int, height: int) -> Dict[str, object]:
    exe = compile_driver()
    output_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        run_capture_case(exe, output_dir, label, control, value, renderer, width, height)
        for label, control, value, renderer in CASES
    ]
    influence = build_influence(cases)
    contact_sheet = make_contact_sheet(output_dir, cases)
    frame_time_log = write_frame_time_log(output_dir, cases)
    contact_markdown = write_markdown(output_dir, contact_sheet, cases)
    status = "pass" if all(row["changed"] for row in influence) else "hold"
    summary = {
        "schema": "screensave.plasma.v3.visual-spike.proof.v0",
        "status": status,
        "dimensions": {"width": width, "height": height},
        "claim_boundary": (
            "deterministic local visual-core spike evidence only; not publication, "
            "runtime compatibility certification, or acceptance"
        ),
        "driver": repo_path(exe),
        "source_files": [repo_path(path) for path in SOURCES],
        "contact_sheet": repo_path(contact_sheet),
        "contact_sheet_markdown": repo_path(contact_markdown),
        "frame_time_log": repo_path(frame_time_log),
        "cases": cases,
        "control_influence": influence,
    }
    (output_dir / "control-influence.json").write_text(
        json.dumps({"status": status, "rows": influence}, indent=2) + "\n",
        encoding="ascii",
    )
    (output_dir / "proof-summary.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="ascii")
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=pathlib.Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--width", type=int, default=192)
    parser.add_argument("--height", type=int, default=108)
    args = parser.parse_args()

    try:
        summary = generate(args.output_dir, args.width, args.height)
    except Exception as exc:
        print(str(exc), file=sys.stderr)
        return 1
    print(f"Plasma V3 visual spike capture {summary['status']} {repo_path(args.output_dir)}")
    return 0 if summary["status"] == "pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())
