# BenchLab Tests

Purpose: lightweight validation for the BenchLab app contract.
Belongs here: truthful smoke checks for app config defaults, target-module wiring, and shared-contract assumptions.
Does not belong here: fake captures, fake benchmark numbers, or heavyweight GUI automation.

Current stage:

- `smoke.c` checks BenchLab defaults and confirms the active target module is a valid saver module.

Type: product validation source.
