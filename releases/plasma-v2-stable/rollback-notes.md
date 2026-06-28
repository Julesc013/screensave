# Plasma v2 Stable Rollback Notes

Publication prep has no installer side effects and publishes nothing. Rollback
is therefore a repository state and release-packet decision, not an
installed-product operation.

Rollback facts:

- If publication prep blocks, keep Plasma v2 stable-promoted and set
  publication prep to hold with exact repair WorkUnits.
- If a later publication gate blocks, do not upload or publish assets.
- Keep `plasma-v2-rc1` artifacts available for comparison.
- Compatibility certification is not broadened, so rollback does not revoke an
  OS certification promise.
- Manager install/apply mutation is not admitted by this packet.
