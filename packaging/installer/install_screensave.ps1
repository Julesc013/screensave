[CmdletBinding()]
param(
    [string]$PackageRoot = "",
    [string]$InstallRoot = "",
    [ValidateSet("current_user")]
    [string]$InstallMode = "current_user",
    [string]$SetActiveSaver = "",
    [string]$MockStatePath = ""
)

$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "installer_common.ps1")

if ([string]::IsNullOrEmpty($PackageRoot)) {
    $PackageRoot = $PSScriptRoot
}

$packageRootResolved = [System.IO.Path]::GetFullPath($PackageRoot)
$manifestPath = Join-Path $packageRootResolved "installer_manifest.ini"
$payloadRoot = Join-Path $packageRootResolved "PAYLOAD"
$manifest = Read-ScreenSaveIni -Path $manifestPath

if ($InstallMode -ne (Get-ScreenSaveManifestValue -Manifest $manifest -Section "install" -Key "supported_mode")) {
    throw "Unsupported install mode: $InstallMode"
}

if (-not (Test-Path -LiteralPath $payloadRoot)) {
    throw "Missing installer payload root: $payloadRoot"
}

$payloadSavers = Get-ScreenSavePayloadSaverSlugs -PayloadRoot $payloadRoot
if ($payloadSavers.Count -eq 0) {
    throw "The installer payload does not contain any saver binaries."
}

if (-not [string]::IsNullOrEmpty($SetActiveSaver) -and ($payloadSavers -notcontains $SetActiveSaver)) {
    throw "The requested saver '$SetActiveSaver' is not present in the current installer payload."
}

if ([string]::IsNullOrEmpty($InstallRoot)) {
    $InstallRoot = Get-ScreenSaveDefaultInstallRoot -Manifest $manifest
}

$installRootResolved = Assert-ScreenSaveInstallTarget -InstallRoot $InstallRoot
$existingState = $null

if (Test-Path -LiteralPath $installRootResolved) {
    $entries = Get-ChildItem -LiteralPath $installRootResolved -Force
    if ($entries.Count -gt 0) {
        $existingState = Get-ScreenSaveExistingInstallState -InstallRoot $installRootResolved
        if ($null -eq $existingState) {
            throw "Refusing to install into a non-empty directory that is not already managed by ScreenSave: $installRootResolved"
        }
    }
}

Copy-ScreenSaveDirectoryContents -SourceRoot $payloadRoot -DestinationRoot $installRootResolved

$installerRoot = Join-Path $installRootResolved "INSTALLER"
New-Item -ItemType Directory -Force -Path $installerRoot | Out-Null

Copy-Item -LiteralPath (Join-Path $packageRootResolved "installer_manifest.ini") -Destination (Join-Path $installerRoot "installer_manifest.ini") -Force
Copy-Item -LiteralPath (Join-Path $packageRootResolved "installer_common.ps1") -Destination (Join-Path $installerRoot "installer_common.ps1") -Force
Copy-Item -LiteralPath (Join-Path $packageRootResolved "install_screensave.ps1") -Destination (Join-Path $installerRoot "install_screensave.ps1") -Force
Copy-Item -LiteralPath (Join-Path $packageRootResolved "uninstall_screensave.ps1") -Destination (Join-Path $installerRoot "uninstall_screensave.ps1") -Force

$installerDocsRoot = Join-Path $packageRootResolved "DOCS"
if (Test-Path -LiteralPath $installerDocsRoot) {
    Copy-ScreenSaveDirectoryContents -SourceRoot $installerDocsRoot -DestinationRoot (Join-Path $installerRoot "DOCS")
}

$state = @{
    package_key = Get-ScreenSaveManifestValue -Manifest $manifest -Section "bundle" -Key "key"
    display_name = Get-ScreenSaveManifestValue -Manifest $manifest -Section "bundle" -Key "display_name"
    version_label = Get-ScreenSaveManifestValue -Manifest $manifest -Section "bundle" -Key "version_label"
    install_mode = $InstallMode
    install_root = $installRootResolved
    package_root = $packageRootResolved
    uninstall_key = Get-ScreenSaveManifestValue -Manifest $manifest -Section "install" -Key "uninstall_key"
    payload_savers = $payloadSavers
    payload_source = Get-ScreenSaveManifestValue -Manifest $manifest -Section "portable_source" -Key "staging_root"
    installed_at = (Get-Date).ToString("s")
    backend = $(if ([string]::IsNullOrEmpty($MockStatePath)) { "registry" } else { "mock" })
    selection_assistance = $(if ([string]::IsNullOrEmpty($SetActiveSaver)) { "manual" } else { "selected" })
}

if ($null -ne $existingState) {
    if ($existingState.ContainsKey("previous_scrnsave_exe")) {
        $state["previous_scrnsave_exe"] = [string]$existingState["previous_scrnsave_exe"]
    }
    if ($existingState.ContainsKey("previous_screensave_active")) {
        $state["previous_screensave_active"] = [string]$existingState["previous_screensave_active"]
    }
    if ($existingState.ContainsKey("selected_saver")) {
        $state["selected_saver"] = [string]$existingState["selected_saver"]
    }
}

if (-not [string]::IsNullOrEmpty($SetActiveSaver)) {
    $state["previous_scrnsave_exe"] = Get-ScreenSaveDesktopValue -DesktopValueName "SCRNSAVE.EXE" -MockStatePath $MockStatePath
    $state["previous_screensave_active"] = Get-ScreenSaveDesktopValue -DesktopValueName "ScreenSaveActive" -MockStatePath $MockStatePath
    $state["selected_saver"] = $SetActiveSaver

    $targetSaverPath = Join-Path (Join-Path $installRootResolved "SAVERS") "$SetActiveSaver.scr"
    Set-ScreenSaveDesktopValue -DesktopValueName "SCRNSAVE.EXE" -Value $targetSaverPath -MockStatePath $MockStatePath
    Set-ScreenSaveDesktopValue -DesktopValueName "ScreenSaveActive" -Value "1" -MockStatePath $MockStatePath
}

$statePath = Join-Path $installerRoot "install-state.json"
Write-ScreenSaveJsonFile -Path $statePath -Data $state

$uninstallScript = Join-Path $installerRoot "uninstall_screensave.ps1"
$uninstallString = 'powershell.exe -ExecutionPolicy Bypass -File "{0}" -InstallRoot "{1}"' -f $uninstallScript, $installRootResolved
if (-not [string]::IsNullOrEmpty($MockStatePath)) {
    $uninstallString = '{0} -MockStatePath "{1}"' -f $uninstallString, $MockStatePath
}

$uninstallEntry = @{
    DisplayName = [string]$state["display_name"]
    DisplayVersion = [string]$state["version_label"]
    Publisher = "ScreenSave"
    InstallLocation = $installRootResolved
    UninstallString = $uninstallString
    QuietUninstallString = $uninstallString
    NoModify = "1"
    NoRepair = "1"
}

Set-ScreenSaveUninstallEntry -UninstallKey ([string]$state["uninstall_key"]) -Values $uninstallEntry -MockStatePath $MockStatePath

Write-Host "Installed ScreenSave to $installRootResolved"
Write-Host ("Staged savers: {0}" -f ($payloadSavers -join ", "))
if (-not [string]::IsNullOrEmpty($SetActiveSaver)) {
    Write-Host "Active saver selection assistance set current-user saver to $SetActiveSaver."
}
else {
    Write-Host "Active saver selection assistance was not requested."
}
if (-not [string]::IsNullOrEmpty($MockStatePath)) {
    Write-Host "Mock registry state written to $MockStatePath"
}
