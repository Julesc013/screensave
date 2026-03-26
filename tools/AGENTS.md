# AGENTS.md

This file applies to `tools/`.

- Tools exist to support the repository; they are not the product runtime.
- Prefer simple, inspectable tooling with explicit inputs and outputs.
- Do not make a tool a mandatory third-party runtime dependency for the shipped savers.
- Keep tooling non-destructive by default and document any side effects clearly.
- When a tool changes repository truth, update nearby docs in the same series.
