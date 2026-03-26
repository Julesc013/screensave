# AGENTS.md

This file applies to `tests/`.

- Tests exist to verify real behavior, not to manufacture confidence.
- Keep fake logs, fake captures, and fake screenshots out of this tree.
- Put executable verification logic here and store supporting evidence in `validation/` when later series produce it.
- Prefer the smallest truthful test that proves the claimed behavior.
- When a test exposes a contract gap, update the relevant spec or doc instead of letting the mismatch linger.
