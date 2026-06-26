# ScreenSave AIDE Evidence Bridge

This bridge projects ScreenSave-owned proof outputs into a compact AIDE-facing
EvidencePacket JSON. It does not rerun image comparison, certify operating
system support, accept artistic quality, or promote a release.

Inputs:

- project-adapter receipt
- Proof Bundle v1 JSON
- adapter artifact manifest
- optional PE audit JSON

Output:

- `screensave.aide-evidence-packet.v0`

The ScreenSave proof bundle remains the domain payload. AIDE receives evidence
references and separated claim axes only.

Portable v2 exports may also attach a fixed equivalence receipt. The packet
then records `domain_payload_ref` for the Proof Bundle v1 payload plus explicit
claims for portable v2 equivalence, v1 Win32 artifact preservation evidence,
no AIDE runtime dependency, compatibility-not-certified, blocked artistic
acceptance, and blocked release promotion.

`index_evidence.py` validates `.aide/evidence/index.toml` and emits a compact
summary. The index records references to Nocturne and Ricochet v1/v2 proofs,
portable v2 equivalence, artifact manifests, PE audit JSON, Proof Bundle v1,
and AIDE EvidencePackets; it does not turn AIDE into proof authority.
