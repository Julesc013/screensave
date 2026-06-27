# Plasma v2 Release-Candidate Rollback Notes

Rollback for PAW-H package staging is source-control based:

1. Remove `packaging/windows/plasma-v2-release-candidate/`.
2. Keep Plasma v2 at `release-readiness-reviewed`.
3. Continue using the PAW-G preview package stage.

No public release asset, installer mutation, or stable promotion is created by
this staging directory. Rollback does not change compatibility certification
status.
