# BenchLab Tests

Purpose: lightweight validation for the BenchLab app contract.
Belongs here: truthful smoke checks for app config defaults, current saver-selection wiring, and shared-contract assumptions.
Does not belong here: fake captures, fake benchmark numbers, or heavyweight GUI automation.

Current stage:

- `smoke.c` checks BenchLab defaults, confirms the default target module is valid, and confirms the current saver set includes Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, and Lifeforms alongside Nocturne.

Type: product validation source.
