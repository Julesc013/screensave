# PX20 Pack And Provenance

## Purpose

This note records the Plasma-local pack, provenance, versioning, and compatibility substrate that `PX20` makes real.

## Shared Versus Plasma-Local Boundary

Shared now:

- the existing shared `pack.ini` shell
- shared `schema_version` and version vocabulary
- shared source-kind vocabulary and safe relative path rules

Plasma-local in `PX20`:

- `pack.provenance.ini`
- support-tier and provenance-kind vocabulary for Plasma packs
- Plasma-specific migration policy wording
- Plasma-specific classic-identity preservation notes

`PX20` does not promote a new shared provenance contract.
That remains local because `SY20` did not justify broader shared promotion.

## Landed Substrate

`PX20` adds `products/savers/plasma/packs/lava_remix/pack.provenance.ini` with:

- explicit pack identity
- content version
- channel and support-tier markers
- source-kind marker
- author and provenance-kind markers
- compatibility markers aligned with the shared `pack.ini`
- explicit migration policy wording
- explicit `preserves_classic_identity=true`

## Migration And Versioning Expectations

- the current sidecar uses `kind=plasma-pack-provenance` and `version=1`
- product key remains `plasma`
- product schema version remains `1`
- unknown or invalid fields should be clamped or rejected rather than silently trusted
- compatibility fields must stay aligned with the owning `pack.ini`

## Current Compatibility Caveats

- `lava_remix` is the only shipped pack with a provenance sidecar in the current repo
- the shared `pack.ini` shell remains the compatibility anchor for pack discovery and import safety
- there is no broad portable or user-pack provenance workflow yet
- there is no repo-grounded shared provenance schema beyond the thin shared pack shell

## Later Work

Later waves may broaden:

- more pack sidecars
- richer provenance chains
- migration tooling
- broader authored content discovery

Those are not current `PX20` claims.
