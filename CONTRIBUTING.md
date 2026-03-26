# Contributing

ScreenSave is a long-lived engineering repository.
The project is still in early infrastructure stages, so contribution discipline matters more than volume.

## Start Here

Before changing anything, read:

- `README.md`
- `AGENTS.md`
- the relevant files under `specs/`
- `docs/roadmap/prompt-program.md`
- any deeper `AGENTS.md` file for the path you plan to touch

The files under `specs/` are authoritative.
If code, docs, workflows, or packaging notes drift from the specs, either fix the drift or update the normative files explicitly in the same series.

## Current Stage

Series 00 established the constitutional layer and repository skeleton.
Series 01 adds governance, contribution, and CI foundations.
There is still no runtime implementation, build system, packaging logic, or releasable product code in the repository.

Do not overstate the repository's maturity in issues, pull requests, workflows, or docs.

## Repository Boundaries

- `platform/` is the shared runtime and public platform surface.
- `products/` is for saver-specific, app-specific, and SDK-specific material.
- `tests/` is for executable verification logic.
- `validation/` is for stored evidence and validation notes.
- `tools/` is for repository support tooling, not shipped runtime dependencies.
- `packaging/` is for release delivery structure and later packaging work, not runtime behavior.
- `docs/` explains the architecture and prompt program.
- `specs/` defines the normative contracts.

Keep shared behavior out of products until it is actually reused.
Do not let product-local needs quietly redefine platform contracts.

## Working In Series

This repository is being developed as a series-based prompt program.
Treat the active series scope as a hard boundary:

- make the smallest durable change that satisfies the current series
- do not pull future-series implementation work forward without need
- keep docs and changelog entries truthful in the same series as the change
- preserve the compatibility contract unless a series explicitly updates it

## Issues And Proposals

Use the issue forms under `.github/ISSUE_TEMPLATE/`.
They are structured to separate:

- bugs
- compatibility findings
- saver ideas
- broader feature requests

When reporting compatibility or runtime behavior later, include real evidence where possible.
Do not file fake screenshots, invented logs, or unsupported compatibility claims.

## Pull Requests

Use the pull request template.
Every PR should:

- summarize scope clearly
- cite the specs and docs consulted
- list the files changed
- describe verification actually performed
- call out risks and contract changes

If a change affects compatibility, renderer behavior, config boundaries, release expectations, or product ownership, update the relevant spec or doc in the same PR.

## Verification Expectations

At this stage, verification is repository-health focused rather than product-build focused.

When touching governance or docs, run the lightweight checks under `tools/scripts/` as relevant:

```powershell
python tools/scripts/check_repo_structure.py
python tools/scripts/check_codex_config.py
python tools/scripts/check_docs_basics.py
python tools/scripts/check_build_layout.py
```

The release scaffold check is for manual release-preparation review, not for publishing artifacts:

```powershell
python tools/scripts/check_release_scaffold.py --track preview
```

If you cannot run a relevant check, say so clearly in the PR.

## Truthfulness Rules

- Do not claim a build passed if no build exists yet.
- Do not claim compatibility that has not been validated.
- Do not add placeholder automation that pretends packaging or release publishing is ready.
- Do not create fake evidence to satisfy a checklist.

The goal is a reviewable, durable repository state after every series.
