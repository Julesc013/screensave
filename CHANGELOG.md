# Changelog

All notable changes to this repository should be recorded here in series order.

## S00 - 2026-03-27

### Added

- Root and subtree `AGENTS.md` control files.
- Repository line-ending policy in `.gitattributes`.
- Project-scoped Codex control metadata in `.codex/config.toml`.
- Initial custom agent briefs in `.codex/agents/`.
- Normative constitution and compatibility specs in `specs/`.
- Architecture, validation, and series-map documents in `docs/`.
- Shared source, product, and tooling directory scaffolding with purpose docs.
- Bootstrap verification script at `tools/verify_bootstrap.ps1`.

### Changed

- Expanded the root `README.md` from a placeholder into a truthful project bootstrap overview.

### Validation

- Ran `.\tools\verify_bootstrap.ps1`.
- Confirmed the environment currently has PowerShell available, while `git`, `cl`, `msbuild`, and `cmake` are not on `PATH`.
