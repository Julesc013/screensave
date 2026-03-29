"""Validate the locked C03 canonical saver/app naming baseline."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

CANONICAL_SAVERS = (
    "nocturne",
    "ricochet",
    "deepfield",
    "plasma",
    "phosphor",
    "pipeworks",
    "lifeforms",
    "signals",
    "mechanize",
    "ecosystems",
    "stormglass",
    "transit",
    "observatory",
    "vector",
    "explorer",
    "city",
    "atlas",
    "gallery",
)

LEGACY_SAVERS = (
    "ember",
    "oscilloscope_dreams",
    "signal_lab",
    "mechanical_dreams",
    "night_transit",
    "retro_explorer",
    "vector_worlds",
    "fractal_atlas",
    "city_nocturne",
    "gl_gallery",
)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def require_text(path: pathlib.Path, snippets: tuple[str, ...], errors: list[str]) -> None:
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        require(snippet in text, f"{path.relative_to(ROOT)} is missing expected text: {snippet!r}", errors)


def main() -> int:
    errors: list[str] = []
    savers_root = ROOT / "products" / "savers"
    apps_root = ROOT / "products" / "apps"

    for saver in CANONICAL_SAVERS:
        require((savers_root / saver).is_dir(), f"Missing canonical saver directory: products/savers/{saver}", errors)

    for saver in LEGACY_SAVERS:
        require(not (savers_root / saver).exists(), f"Legacy saver directory still present: products/savers/{saver}", errors)

    require((apps_root / "benchlab").is_dir(), "Missing canonical app directory: products/apps/benchlab", errors)
    require((apps_root / "suite").is_dir(), "Missing canonical app directory: products/apps/suite", errors)
    require((apps_root / "player").is_dir(), "Missing superseded app stub: products/apps/player", errors)
    require(not (apps_root / "gallery").exists(), "Legacy app directory still present: products/apps/gallery", errors)

    require_text(
        ROOT / "README.md",
        (
            "Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, and `C03`.",
            "The next continuation phase is `C04` for shared settings, presets, randomization, and pack architecture.",
            "`products/apps/suite/` is now the canonical suite-app placeholder location.",
        ),
        errors,
    )
    require_text(
        ROOT / "docs" / "roadmap" / "prompt-program.md",
        (
            "Post-`S15` work now follows committed `C00`, `C01`, `C02`, and `C03` work plus later continuation prompts.",
            "C04 is the next planned implementation prompt after saver productization.",
            "- `C00`, `C01`, `C02`, and `C03` are complete.",
        ),
        errors,
    )
    require_text(
        ROOT / "docs" / "roadmap" / "series-map.md",
        (
            "`C00`, `C01`, `C02`, and `C03` are complete.",
            "The next planned implementation prompt is `C04` shared settings, presets, randomization, and pack architecture.",
        ),
        errors,
    )
    require_text(
        ROOT / "docs" / "roadmap" / "post-s15-plan.md",
        (
            "the locked `C02` post-rename checkpoint",
            "The next continuation prompt should be `C04` for shared settings, presets, randomization, and pack architecture.",
        ),
        errors,
    )
    require_text(
        ROOT / "docs" / "roadmap" / "rename-map.md",
        (
            "landed in `C01` and was locked as the post-rename checkpoint in `C02`",
        ),
        errors,
    )
    require_text(
        ROOT / "validation" / "notes" / "c02-migration-baseline.md",
        (
            "# C02 Migration Baseline",
            "- `C03` all-saver `.scr` productization",
            "- Annotated git tag for this baseline: `c02-post-rename-baseline`",
        ),
        errors,
    )
    require_text(
        ROOT / "validation" / "notes" / "c03-productization-matrix.md",
        (
            "# C03 Productization Matrix",
            "- `C04` shared settings, presets, randomization, and pack architecture",
            "- BenchLab remains a separate diagnostics app",
        ),
        errors,
    )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Canonical naming checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
