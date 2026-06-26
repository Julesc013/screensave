# screensave Open Risks

- Risk: ScreenSave-local AIDE bootstrap is incomplete after safe import.
  - Status: active
  - Mitigation: add repository-local token baselines, controller taxonomy, bootstrap wrapper, and validator before treating the AIDE loop as routine.

- Risk: DistributionManifest v1 in AIDE remains unaccepted.
  - Status: active
  - Mitigation: keep AIDE pinned, disable self-update and automatic target update machinery, and treat ScreenSave-local bridge work as separate from AIDE-repository canonical acceptance.

- Risk: General worker boundaries could be confused with fixed capability invocation.
  - Status: active
  - Mitigation: keep source mutation, preview/apply, automatic merge, and worker-host semantics explicitly blocked in pilot, bridge, validators, and state files.
