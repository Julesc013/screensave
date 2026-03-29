[CmdletBinding()]
param(
    [string]$PackageRoot = "",
    [string]$InstallRoot = "",
    [string]$MockStatePath = ""
)

$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "installer_common.ps1")

if ([string]::IsNullOrEmpty($PackageRoot)) {
    $PackageRoot = $PSScriptRoot
}

$packageRootResolved = [System.IO.Path]::GetFullPath($PackageRoot)
$manifestPath = Join-Path $packageRootResolved "installer_manifest.ini"
$manifest = Read-ScreenSaveIni -Path $manifestPath

if ([string]::IsNullOrEmpty($InstallRoot)) {
    $localStatePath = Join-Path $packageRootResolved "install-state.json"
    if (Test-Path -LiteralPath $localStatePath) {
        $localState = Read-ScreenSaveJsonFile -Path $localStatePath
        $InstallRoot = [string]$localState["install_root"]
    }
    else {
        $InstallRoot = Get-ScreenSaveDefaultInstallRoot -Manifest $manifest
    }
}

$installRootResolved = Assert-ScreenSaveManagedInstallRoot -InstallRoot $InstallRoot
$statePath = Join-Path $installRootResolved "INSTALLER\install-state.json"
$state = Read-ScreenSaveJsonFile -Path $statePath

$currentSaverPath = Get-ScreenSaveDesktopValue -DesktopValueName "SCRNSAVE.EXE" -MockStatePath $MockStatePath
$saversRoot = Join-Path $installRootResolved "SAVERS"
$currentSaverUnderInstall = $false

if (-not [string]::IsNullOrEmpty($currentSaverPath)) {
    $normalizedCurrent = [System.IO.Path]::GetFullPath($currentSaverPath)
    $normalizedSaversRoot = ([System.IO.Path]::GetFullPath($saversRoot)).TrimEnd('\') + "\"
    if ($normalizedCurrent.StartsWith($normalizedSaversRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        $currentSaverUnderInstall = $true
    }
}

if ($currentSaverUnderInstall) {
    if ($state.ContainsKey("previous_scrnsave_exe") -or $state.ContainsKey("previous_screensave_active")) {
        $previousPath = ""
        $previousActive = "0"
        if ($state.ContainsKey("previous_scrnsave_exe")) {
            $previousPath = [string]$state["previous_scrnsave_exe"]
        }
        if ($state.ContainsKey("previous_screensave_active")) {
            $previousActive = [string]$state["previous_screensave_active"]
        }

        Set-ScreenSaveDesktopValue -DesktopValueName "SCRNSAVE.EXE" -Value $previousPath -MockStatePath $MockStatePath
        Set-ScreenSaveDesktopValue -DesktopValueName "ScreenSaveActive" -Value $previousActive -MockStatePath $MockStatePath
    }
    else {
        Set-ScreenSaveDesktopValue -DesktopValueName "SCRNSAVE.EXE" -Value "" -MockStatePath $MockStatePath
        Set-ScreenSaveDesktopValue -DesktopValueName "ScreenSaveActive" -Value "0" -MockStatePath $MockStatePath
    }
}

Remove-ScreenSaveUninstallEntry -UninstallKey ([string]$state["uninstall_key"]) -MockStatePath $MockStatePath
Remove-Item -LiteralPath $installRootResolved -Recurse -Force

Write-Host "Removed ScreenSave install root $installRootResolved"
Write-Host "User configuration and future user-pack roots outside the install tree were preserved."
if (-not [string]::IsNullOrEmpty($MockStatePath)) {
    Write-Host "Mock registry state updated at $MockStatePath"
}
