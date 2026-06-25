# Renderer Alias Contract v1

Status: active compatibility contract.

## Purpose

Existing renderer names are embedded in manifests, settings, routing policy,
docs, tests, diagnostics, and user expectations. They must not be abruptly
removed while ScreenSave grows a richer host-neutral architecture.

## Public Compatibility Aliases

| Alias | Meaning |
| --- | --- |
| `gdi` | software/reference renderer plus Win32 GDI presenter |
| `gl11` | OpenGL 1.1 realization plus WGL presenter/context |
| `gl21` | OpenGL 2.1 realization plus WGL presenter/context |
| `gl33` | OpenGL 3.3 realization plus WGL presenter/context |
| `gl46` | OpenGL 4.6 realization plus WGL presenter/context |
| `null` | semantic execution with no presentation; internal safety/headless use only unless a later spec widens it |

## Internal Axes

Future v2 routing may reason about these axes independently:

- compute provider
- rendering realization
- surface type
- presenter
- host adapter
- OS service adapter

Those axes are internal until a later public spec promotes them.

## Preservation Rule

Saver manifests, pack metadata, diagnostics, and existing routing docs continue
to use the public aliases.

Private routing may map aliases onto richer providers, but public claims must
remain explainable through the alias ladder:

```text
gl46 -> gl33 -> gl21 -> gl11 -> gdi -> null
```

`gdi` remains mandatory for Core savers. Higher aliases remain optional and
capability-gated.
