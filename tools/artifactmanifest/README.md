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

Feed a passing manifest into the PE auditor when a proof or release lane needs
exact artifact membership and profile policy:

```powershell
python tools\scripts\audit_pe_artifacts.py --artifact-manifest out\proof\artifact-manifests\windows_current_x86_scr.json --artifact-profile windows_current_x86_scr --output out\proof\manifest-pe-audit.txt --json-output out\proof\manifest-pe-audit.json
```

That mode blocks missing manifests, missing expected artifacts, unexpected
artifacts, empty artifact sets, parse failures, and profile mismatches instead
of treating a loose directory scan as proof.

Type: repository proof tooling.
