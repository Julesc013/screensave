# Pack v1 Contract

Status: bounded Pack v1 preview contract.

`packc` compiles repository-local authoring TOML into a deterministic,
data-only runtime package. Pack v1 is not a plugin ABI, not a scripting surface,
not a public SDK freeze, and not a release-promotion mechanism.

## Scope

The first admitted pack kind is:

- `screensave.plasma.v2`

Pack v1 can carry one bounded `plasma_spec_v2` payload plus provenance,
license, and a proof-profile reference. It does not compile source code or load
runtime behavior.

## Pipeline

The compiler pipeline is:

1. TOML authoring file
2. schema validation
3. token and ID resolution
4. canonical JSON ordering
5. bounded runtime package emission
6. hash manifest emission
7. proof-profile reference emission

## Safety Rules

Pack v1 is data only:

- no scripts,
- no DLLs or executables,
- no macros,
- no network references,
- no absolute paths,
- no parent traversal,
- bounded strings,
- bounded file count,
- bounded expanded size,
- explicit provenance,
- explicit license,
- transactional output under `out/`.

The compiler must reject unknown pack kinds, unknown Plasma v2 tokens,
out-of-range numeric fields, network references, path-like string payloads, and
executable-looking fields.

## Preview Schemas

The Pack v1 preview schema files are:

- `tools/packc/schemas/plasma_spec_v2.schema.json`
- `tools/packc/schemas/pack_manifest_v1.schema.json`
- `tools/packc/schemas/plasma_pack_v1.schema.json`

These schemas document the bounded compiler surface. The compiler itself remains
the local deterministic authority for accepted tokens, canonical output, content
hashes, proof-profile references, and output-root enforcement.

## Boundaries

Pack v1 does not imply:

- VisualIntent text-to-code generation,
- runtime executable plugins,
- compatibility certification,
- artistic acceptance,
- stable release promotion,
- AIDE worker authority.
