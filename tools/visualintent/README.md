# VisualIntent Tools

`resolve_plasma.py` is a deterministic local resolver for the PAW-E Plasma v2
preview lane.

It accepts VisualIntent v1 TOML and emits three bounded Plasma v2 Pack v1
preview candidates, then compiles each candidate through `packc`.

It does not call models or the network, generate source code, admit runtime
plugins, make stable claims, or promote candidates automatically.

Example:

```powershell
py -3 tools\visualintent\resolve_plasma.py resolve tools\visualintent\examples\plasma_warm_room.toml --out out\visualintent\plasma_warm_room
```
