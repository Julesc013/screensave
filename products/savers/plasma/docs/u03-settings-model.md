# U03 Settings Model

`U03` recuts Plasma's settings model into three truthful layers.

## Basic

Basic now keeps only the controls that are both meaningful and end-user-safe:

- preset
- theme
- speed
- visual intensity
- content pool
- transitions enabled

That keeps the first surface focused on identity, pace, and stable-versus-experimental breadth without exposing half-real lab plumbing.

## Advanced

Advanced now adds the controls that directly own the visual grammar:

- generator family
- output family
- output mode
- field resolution
- smoothing
- filter treatment
- emulation treatment
- accent treatment
- presentation mode
- deterministic seed mode

These controls now map to persisted config fields and compiled plan fields, so the surface matches runtime truth.

## Author/Lab

Author/Lab now carries the curation and transition controls that are useful but not first-pass:

- preset set
- theme set
- favorites only
- transition policy
- journey
- transition fallback
- transition seed continuity
- transition interval and duration
- deterministic seed value
- diagnostics overlay

## Explicit Non-Claims

- `sampling_treatment` remains hidden because it still clamps to `none`.
- favorites and exclusion key lists remain file-first and import/export-first; the dialog exposes the bounded `favorites_only` gate but not a new list editor.
- no extra broad feature family was added for this UI pass.
