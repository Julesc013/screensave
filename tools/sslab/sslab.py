"""ScreenSave Lab Runner v0 for deterministic proof-kernel captures."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys
from dataclasses import dataclass


ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_OUTPUT = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne"

FIXED_ONE = 65536
FIXED_HALF = 32768

MOTION_NONE = 0
MOTION_DRIFT_MARK = 1
MOTION_QUIET_LINE = 2
MOTION_MONOLITH = 3
MOTION_BREATH = 4

FADE_SLOW = 0
FADE_STANDARD = 1
FADE_GENTLE = 2

STRENGTH_STILL = 0
STRENGTH_SUBTLE = 1
STRENGTH_SOFT = 2


def display_path(path: pathlib.Path) -> str:
    """Use repo-relative proof paths when possible and absolute temp paths otherwise."""
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


@dataclass
class Color:
    red: int
    green: int
    blue: int
    alpha: int = 255


@dataclass
class Rect:
    x: int
    y: int
    width: int
    height: int


@dataclass
class Point:
    x: int
    y: int


class Rng:
    def __init__(self, seed: int) -> None:
        self.state = seed if seed != 0 else 0x0A1E0A1E

    def next(self) -> int:
        self.state = ((self.state * 1664525) + 1013904223) & 0xFFFFFFFF
        return self.state

    def range(self, upper_bound: int) -> int:
        if upper_bound <= 0:
            return 0
        return self.next() % upper_bound


class Surface:
    def __init__(self, width: int, height: int) -> None:
        if width <= 0 or height <= 0:
            raise ValueError("surface dimensions must be positive")
        self.width = width
        self.height = height
        self.pixels = bytearray(width * height * 4)

    def set_pixel(self, x: int, y: int, color: Color) -> None:
        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return
        offset = ((y * self.width) + x) * 4
        self.pixels[offset + 0] = color.red & 0xFF
        self.pixels[offset + 1] = color.green & 0xFF
        self.pixels[offset + 2] = color.blue & 0xFF
        self.pixels[offset + 3] = color.alpha & 0xFF

    def clear(self, color: Color) -> None:
        for y in range(self.height):
            for x in range(self.width):
                self.set_pixel(x, y, color)

    def fill_rect(self, rect: Rect, color: Color) -> None:
        if rect.width <= 0 or rect.height <= 0:
            return
        x0 = max(rect.x, 0)
        y0 = max(rect.y, 0)
        x1 = min(rect.x + rect.width, self.width)
        y1 = min(rect.y + rect.height, self.height)
        for y in range(y0, y1):
            for x in range(x0, x1):
                self.set_pixel(x, y, color)

    def frame_rect(self, rect: Rect, color: Color) -> None:
        if rect.width <= 0 or rect.height <= 0:
            return
        self.fill_rect(Rect(rect.x, rect.y, rect.width, 1), color)
        self.fill_rect(Rect(rect.x, rect.y + rect.height - 1, rect.width, 1), color)
        self.fill_rect(Rect(rect.x, rect.y, 1, rect.height), color)
        self.fill_rect(Rect(rect.x + rect.width - 1, rect.y, 1, rect.height), color)

    def line(self, start: Point, end: Point, color: Color) -> None:
        x0 = start.x
        y0 = start.y
        x1 = end.x
        y1 = end.y
        dx = abs(x1 - x0)
        dy = -abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx + dy
        while True:
            self.set_pixel(x0, y0, color)
            if x0 == x1 and y0 == y1:
                break
            e2 = err * 2
            if e2 >= dy:
                err += dy
                x0 += sx
            if e2 <= dx:
                err += dx
                y0 += sy

    def polyline(self, points: list[Point], color: Color) -> None:
        for index in range(1, len(points)):
            self.line(points[index - 1], points[index], color)

    def sha256(self) -> str:
        return hashlib.sha256(self.pixels).hexdigest()

    def write_ppm(self, path: pathlib.Path) -> None:
        lines = ["P3", f"{self.width} {self.height}", "255"]
        for y in range(self.height):
            row: list[str] = []
            for x in range(self.width):
                offset = ((y * self.width) + x) * 4
                row.append(str(self.pixels[offset + 0]))
                row.append(str(self.pixels[offset + 1]))
                row.append(str(self.pixels[offset + 2]))
            lines.append(" ".join(row))
        path.write_text("\n".join(lines) + "\n", encoding="ascii")


@dataclass
class NocturneSession:
    width: int
    height: int
    seed: int
    preset: str
    theme_key: str
    primary_color: Color
    accent_color: Color
    detail_level: str
    motion_mode: int
    fade_speed: int
    motion_strength: int
    rng: Rng
    cycle_index: int = 0
    cycle_duration_millis: int = 70000
    stage_elapsed_millis: int = 0
    drift_refresh_millis: int = 0
    reseed_count: int = 0
    stage: int = 0
    fade_level: int = 0
    primary_x: int = 0
    primary_y: int = 0
    primary_vx: int = 0
    primary_vy: int = 0
    secondary_x: int = 0
    secondary_y: int = 0
    secondary_vx: int = 0
    secondary_vy: int = 0
    breath_direction: int = 1
    breath_level: int = 64
    ambient_level: int = 32


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def scale_color(color: Color, fade_level: int, motion_strength: int, preview_mode: bool) -> Color:
    scale = fade_level
    if motion_strength == STRENGTH_STILL:
        scale = (scale * 3) // 4
    elif motion_strength == STRENGTH_SOFT:
        scale = (scale * 5) // 4
    if preview_mode:
        scale = (scale * 3) // 4
    scale = min(scale, 255)
    return Color((color.red * scale) // 255, (color.green * scale) // 255, (color.blue * scale) // 255, color.alpha)


def scale_color_amount(color: Color, scale: int) -> Color:
    scale = min(scale, 255)
    return Color((color.red * scale) // 255, (color.green * scale) // 255, (color.blue * scale) // 255, color.alpha)


def speed_units(motion_strength: int, preview_mode: bool) -> int:
    if motion_strength == STRENGTH_STILL:
        speed = 180
    elif motion_strength == STRENGTH_SOFT:
        speed = 420
    else:
        speed = 300
    if preview_mode:
        speed = (speed * 3) // 4
    return speed


def random_velocity(rng: Rng, motion_strength: int, preview_mode: bool) -> int:
    speed = speed_units(motion_strength, preview_mode)
    return speed if (rng.next() & 1) else -speed


def fade_units_per_second(fade_speed: int) -> int:
    if fade_speed == FADE_SLOW:
        return 52
    if fade_speed == FADE_GENTLE:
        return 72
    return 110


def make_nocturne_session(width: int, height: int, seed: int, preset: str) -> NocturneSession:
    if preset != "observatory_night":
        raise ValueError("Proof Kernel v0 supports the Nocturne observatory_night canary only")
    session_seed = (seed ^ 0x0A1E0A1E) & 0xFFFFFFFF
    session = NocturneSession(
        width=width,
        height=height,
        seed=seed,
        preset=preset,
        theme_key="gray_black",
        primary_color=Color(8, 8, 8, 255),
        accent_color=Color(20, 20, 20, 255),
        detail_level="standard",
        motion_mode=MOTION_MONOLITH,
        fade_speed=FADE_GENTLE,
        motion_strength=STRENGTH_SUBTLE,
        rng=Rng(session_seed),
    )
    reset_cycle(session)
    return session


def reset_cycle(session: NocturneSession) -> None:
    session.cycle_index += 1
    session.stage = 0
    session.stage_elapsed_millis = 0
    session.drift_refresh_millis = 0
    session.fade_level = 0
    session.cycle_duration_millis = 70000
    session.primary_vx = random_velocity(session.rng, session.motion_strength, False)
    session.primary_vy = random_velocity(session.rng, session.motion_strength, False)
    session.secondary_vx = random_velocity(session.rng, session.motion_strength, False)
    session.secondary_vy = random_velocity(session.rng, session.motion_strength, False)
    session.breath_direction = 1
    session.breath_level = 48 + session.rng.range(48)
    session.ambient_level = 28 + session.rng.range(28)
    set_initial_positions(session)


def set_initial_positions(session: NocturneSession) -> None:
    session.primary_x = FIXED_HALF + session.rng.range(max(session.width - 1, 1)) * FIXED_ONE
    session.primary_y = FIXED_HALF + session.rng.range(max(session.height - 1, 1)) * FIXED_ONE
    session.secondary_x = FIXED_HALF + session.rng.range(max(session.width - 1, 1)) * FIXED_ONE
    session.secondary_y = FIXED_HALF + session.rng.range(max(session.height - 1, 1)) * FIXED_ONE
    if session.width <= 1:
        session.primary_x = (session.width * FIXED_ONE) // 2
        session.secondary_x = session.primary_x
    if session.height <= 1:
        session.primary_y = (session.height * FIXED_ONE) // 2
        session.secondary_y = session.primary_y


def advance_axis(position: int, velocity: int, limit: int, delta_millis: int) -> tuple[int, int]:
    minimum = FIXED_HALF
    maximum = (max(limit - 1, 1) * FIXED_ONE) - FIXED_HALF
    position += velocity * delta_millis
    if position < minimum:
        return minimum, -velocity
    if position > maximum:
        return maximum, -velocity
    return position, velocity


def step_session(session: NocturneSession, delta_millis: int) -> None:
    delta_millis = min(delta_millis, 200)
    session.primary_x, session.primary_vx = advance_axis(session.primary_x, session.primary_vx, session.width, delta_millis)
    session.primary_y, session.primary_vy = advance_axis(session.primary_y, session.primary_vy, session.height, delta_millis)
    session.secondary_x, session.secondary_vx = advance_axis(session.secondary_x, session.secondary_vx, session.width, delta_millis)
    session.secondary_y, session.secondary_vy = advance_axis(session.secondary_y, session.secondary_vy, session.height, delta_millis)

    fade_delta = (delta_millis * fade_units_per_second(session.fade_speed)) // 1000
    if fade_delta == 0 and delta_millis > 0:
        fade_delta = 1
    session.stage_elapsed_millis += delta_millis
    if session.stage == 0:
        session.fade_level += fade_delta
        if session.fade_level >= 255:
            session.fade_level = 255
            session.stage = 1
            session.stage_elapsed_millis = 0
    elif session.stage == 1:
        if session.stage_elapsed_millis >= session.cycle_duration_millis:
            session.stage = 2
            session.stage_elapsed_millis = 0
    else:
        session.fade_level -= fade_delta
        if session.fade_level <= 0:
            session.fade_level = 0
            session.reseed_count += 1
            reset_cycle(session)


def fixed_to_int(value: int) -> int:
    return value // FIXED_ONE


def render_monolith_at(surface: Surface, center_x: int, center_y: int, width: int, height: int, primary: Color, accent: Color) -> None:
    rect = Rect(center_x - (width // 2), center_y - (height // 2), width, height)
    surface.fill_rect(rect, primary)
    surface.frame_rect(rect, accent)


def render_session(session: NocturneSession) -> Surface:
    surface = Surface(session.width, session.height)
    surface.clear(Color(0, 0, 0, 255))
    primary = scale_color(session.primary_color, session.fade_level, session.motion_strength, False)
    accent = scale_color(session.accent_color, session.fade_level, session.motion_strength, False)

    width = 6
    height = max(session.height // 3, 12)
    ghost_fill = scale_color_amount(primary, 28 + session.ambient_level)
    ghost_outline = scale_color_amount(accent, 52 + session.ambient_level)
    render_monolith_at(surface, fixed_to_int(session.secondary_x), fixed_to_int(session.secondary_y), width, (height * 9) // 10, ghost_fill, ghost_outline)
    render_monolith_at(surface, fixed_to_int(session.primary_x), fixed_to_int(session.primary_y), width, height, primary, accent)

    frame_rect = Rect(6, 6, session.width - 12, session.height - 12)
    if frame_rect.width > 0 and frame_rect.height > 0:
        surface.frame_rect(frame_rect, scale_color_amount(accent, 18 + session.ambient_level))
    return surface


def render_nocturne(args: argparse.Namespace) -> dict:
    output_dir = pathlib.Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    session = make_nocturne_session(args.width, args.height, args.seed, args.preset)
    for _ in range(args.frames):
        step_session(session, args.delta_ms)
    surface = render_session(session)

    capture_path = output_dir / "capture.ppm"
    proof_path = output_dir / "proof.json"
    hash_path = output_dir / "capture.sha256"
    surface.write_ppm(capture_path)
    capture_hash = surface.sha256()
    hash_path.write_text(capture_hash + "\n", encoding="ascii")

    proof = {
        "proof_schema": "proof-bundle-v0",
        "proof_kernel": "proof-kernel-v0",
        "status": "informational",
        "claim_boundary": "deterministic canary capture; not compatibility certification",
        "source": {
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "runtime": {
            "runner": "tools/sslab/sslab.py",
            "product": "nocturne",
            "preset": args.preset,
            "theme": session.theme_key,
            "width": args.width,
            "height": args.height,
            "seed": args.seed,
            "frames": args.frames,
            "delta_ms": args.delta_ms,
            "surface": "rgba8-top-left-srgb",
            "renderer": "soft-reference-v0",
        },
        "capture": {
            "path": display_path(capture_path),
            "sha256": capture_hash,
            "format": "ppm-p3-rgb-from-rgba8",
        },
        "limits": [
            "Nocturne canary only",
            "Python proof runner mirrors the current Nocturne monolith path; it is not a public runtime ABI",
            "No operating-system compatibility claim is certified by this proof",
        ],
    }
    proof_path.write_text(json.dumps(proof, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return proof


def add_render_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("render", help="Render a deterministic proof-kernel canary capture.")
    parser.add_argument("--product", default="nocturne", choices=["nocturne"])
    parser.add_argument("--preset", default="observatory_night")
    parser.add_argument("--width", type=int, default=96)
    parser.add_argument("--height", type=int, default=54)
    parser.add_argument("--seed", type=int, default=1536)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--delta-ms", type=int, default=100)
    parser.add_argument("--output-dir", default=str(DEFAULT_OUTPUT.relative_to(ROOT)))
    parser.set_defaults(func=render_nocturne)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    add_render_parser(subparsers)
    args = parser.parse_args()

    proof = args.func(args)
    print(f"{proof['runtime']['product']} {proof['capture']['sha256']} {proof['capture']['path']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
