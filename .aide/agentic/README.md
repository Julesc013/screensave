# ScreenSave AIDE Agentic Proposal Protocol

The PAW-G agentic loop is proposal-only.

Allowed now:

- compile task context
- generate implementation plans
- generate patch proposals
- generate validation plans
- generate repair proposals
- generate review summaries
- record proposal receipts
- invoke fixed project-owned checks

Forbidden now:

- direct AIDE source mutation
- automatic apply
- automatic merge
- automatic release
- worker self-approval
- compatibility certification
- final artistic acceptance

Suggested local workflow:

```powershell
py -3 tools\aideops\agentic.py propose `
  --task SS-G-123 `
  --objective "Repair Plasma v2 package stage validator" `
  --out out\aide\agentic\SS-G-123
```

The proposal receipt may guide a human or assistant-assisted coding session.
Consequential repository edits remain outside AIDE's autonomous authority and
must pass normal WorkUnit postflight and commit discipline.
