# artifactmanifest

Purpose: generate exact artifact-set manifests from catalog authority.

`artifactmanifest.py` reads `catalog/artifact_sets.toml`, resolves a named
artifact set, and compares the expected artifact paths with the files observed
under the declared root.

Example:

```powershell
python tools\artifactmanifest\artifactmanifest.py generate --artifact-set windows_current_x86_scr --output out\proof\artifact-manifests\windows_current_x86_scr.json
```

The output records source revision, dirty state, build lane, expected artifacts,
observed artifacts, missing artifacts, unexpected artifacts, sizes, and SHA-256
hashes. It does not parse PE headers and does not certify compatibility; PE
policy remains the job of `tools/scripts/audit_pe_artifacts.py`.

Type: repository proof tooling.
