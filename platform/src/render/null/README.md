# Null Backend

Purpose: internal emergency renderer fallback when no drawable backend can be created.
Belongs here: minimal no-op backend code that keeps host and BenchLab lifecycles alive when all real renderer creation fails.
Does not belong here: product logic, normal user-facing renderer selection, or any attempt to redefine the required GDI floor.

Current stage:

- `null_backend.c` creates a tiny no-op renderer instance, reports the active backend as `null`, and suppresses all drawing safely
- `null_internal.h` keeps the emergency fallback hook private to the renderer runtime

This backend is not a normal feature tier. It exists only as the ultimate safety fallback beneath GDI.
Type: runtime implementation.
