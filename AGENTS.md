# AGENTS.md

This file applies to the entire repository unless a deeper `AGENTS.md` overrides it.

## Mission

Build ScreenSave as a coherent, reconstructable engineering project.
Do not treat the empty starting tree as a reason to stop.

## Execution Rules

- Work series in order and prefer narrow repairs over broad rewrites.
- Make at least one real commit per series using an `SNN ...` commit message.
- Keep `CHANGELOG.md`, `README.md`, and affected docs/specs truthful in the same series as the code change.
- Use non-interactive git commands and do not rewrite history during the rollout.

## Compatibility Baseline

- Keep the codebase compatible with C89.
- Treat x86 Win32 `.scr` output as the product baseline.
- Keep GDI as the universal renderer floor.
- Treat GL11 as optional and conservative.
- Keep advanced OpenGL optional and capability-gated.

## Verification

- Review changed files after each series.
- Run the strongest meaningful validation available for the current stage.
- Record toolchain or environment limits honestly in `docs/validation.md`.
