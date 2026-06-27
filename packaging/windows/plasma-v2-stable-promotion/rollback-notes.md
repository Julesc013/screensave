# Plasma v2 Stable-Promotion Rollback Notes

Stable-promotion staging has no installer side effects and publishes nothing. Rollback is therefore a repository state decision rather than an installed-product operation.

Rollback facts:

- Revert the stable state transition if the checker later blocks.
- Keep `plasma-v2-rc1` artifacts available for comparison.
- Preserve repair WorkUnits for any blocker.
- Compatibility certification is not claimed, so rollback does not revoke an OS certification promise.
