# screensave Decisions

- Decision: Admit the clean AIDE Lite safe pack pinned at `492faa4f1a8280ba67954aa4fc252e79f2e19c15`.
  - Status: active
  - Rationale: ScreenSave wants AIDE Lite governance, context, repo intelligence, and review without admitting a general worker or product/runtime dependency.

- Decision: Keep fixed ScreenSave capability invocation local and bounded.
  - Status: active
  - Rationale: ScreenSave-owned fixed adapter commands are usable now; canonical AIDE-repository bridge acceptance can remain serialized separately.

- Decision: Keep worker semantics blocked.
  - Status: active
  - Rationale: Accepted AIDE WorkerRun remains fixture-backed and does not admit source mutation, preview/apply, automatic merge, or a general ScreenSave worker.
