# Scripts

Purpose: general repository support scripts.
Belongs here: helper scripts for validation, maintenance, and local workflow support.
Does not belong here: generated outputs or runtime code shipped with savers.
Current stage:
- `check_repo_structure.py` validates required governance files and workflow anchors.
- `check_codex_config.py` validates the project-scoped Codex configuration.
- `check_docs_basics.py` checks markdown links and roadmap/doc basics.
- `check_build_layout.py` validates the checked-in build scaffold and target relationships.
- `check_release_scaffold.py` generates a manual release-scaffold report without publishing anything.
Type: tooling.
