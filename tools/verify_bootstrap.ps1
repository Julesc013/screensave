param()

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot

$requiredFiles = @(
    "AGENTS.md",
    "README.md",
    "CHANGELOG.md",
    ".gitattributes",
    ".codex/config.toml",
    ".codex/agents/constitution_guard.md",
    ".codex/agents/compatibility_auditor.md",
    "docs/architecture.md",
    "docs/series-map.md",
    "docs/validation.md",
    "specs/constitution.md",
    "specs/compatibility.md",
    "src/README.md",
    "src/common/README.md",
    "src/win32/README.md",
    "src/render/gdi/README.md",
    "src/render/gl11/README.md",
    "products/README.md",
    "tools/README.md"
)

$missingFiles = @()
foreach ($relativePath in $requiredFiles) {
    $fullPath = Join-Path $repoRoot $relativePath
    if (-not (Test-Path -LiteralPath $fullPath)) {
        $missingFiles += $relativePath
    }
}

if ($missingFiles.Count -gt 0) {
    Write-Error ("Missing required bootstrap files:`n- " + ($missingFiles -join "`n- "))
}

$contentChecks = @{
    "README.md" = @("C89", "Win32", ".scr", "GDI", "GL11")
    "specs/compatibility.md" = @("C89", "GDI", "GL11", "capability-gated")
    "docs/series-map.md" = @("S00", "Pending")
}

foreach ($entry in $contentChecks.GetEnumerator()) {
    $fullPath = Join-Path $repoRoot $entry.Key
    $content = Get-Content -LiteralPath $fullPath -Raw
    foreach ($needle in $entry.Value) {
        if ($content -notmatch [regex]::Escape($needle)) {
            Write-Error ("File '" + $entry.Key + "' is missing expected text '" + $needle + "'.")
        }
    }
}

Write-Host "Bootstrap verification passed."
