"""Validate committed cataloggen outputs."""

from __future__ import annotations

import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]


def main() -> int:
    result = subprocess.run(
        [sys.executable, "tools/cataloggen/cataloggen.py", "check"],
        cwd=ROOT,
        text=True,
    )
    return int(result.returncode)


if __name__ == "__main__":
    raise SystemExit(main())
