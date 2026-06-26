# Performance Envelope v1

Status: active evidence contract for stable-candidate review.

Performance Envelope v1 records bounded resource and timing evidence for a
named ScreenSave product slice. It is evidence for candidate review, not a
public performance promise, compatibility certification, or release promotion.

## Required Metrics

An envelope should record:

- `frame_time_ms_p50`
- `frame_time_ms_p95`
- `frame_time_ms_p99`
- `first_frame_ms`
- `memory_high_water`
- `allocation_growth`
- `resize_cycle_result`
- `short_soak_result`
- `fallback_frequency`

If a metric cannot be measured in the current proof runner, the envelope must
say so explicitly and keep the result as a warning rather than a hidden pass.

## Boundary

Performance evidence supports stable-candidate review only. It does not imply
stable release, final artistic acceptance, compatibility certification, or
support for every acceleration lane.
