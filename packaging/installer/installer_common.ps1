Set-StrictMode -Version 2.0

function ConvertTo-ScreenSaveHashtable {
    param(
        [Parameter(Mandatory = $true)]
        [object]$InputObject
    )

    if ($null -eq $InputObject) {
        return $null
    }

    if ($InputObject -is [System.Collections.IDictionary]) {
        $table = @{}
        foreach ($key in $InputObject.Keys) {
            $table[[string]$key] = ConvertTo-ScreenSaveHashtable -InputObject $InputObject[$key]
        }
        return $table
    }

    if ($InputObject -is [pscustomobject]) {
        $table = @{}
        foreach ($property in $InputObject.PSObject.Properties) {
            $table[[string]$property.Name] = ConvertTo-ScreenSaveHashtable -InputObject $property.Value
        }
        return $table
    }

    if (($InputObject -is [System.Collections.IEnumerable]) -and -not ($InputObject -is [string])) {
        $items = @()
        foreach ($item in $InputObject) {
            $items += ,(ConvertTo-ScreenSaveHashtable -InputObject $item)
        }
        return $items
    }

    return $InputObject
}

function Read-ScreenSaveIni {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Missing INI file: $Path"
    }

    $document = @{}
    $sectionName = ""

    foreach ($rawLine in [System.IO.File]::ReadAllLines($Path)) {
        $line = $rawLine.Trim()
        if ($line.Length -eq 0) {
            continue
        }
        if ($line.StartsWith(";") -or $line.StartsWith("#")) {
            continue
        }
        if ($line.StartsWith("[") -and $line.EndsWith("]")) {
            $sectionName = $line.Substring(1, $line.Length - 2).Trim()
            if (-not $document.ContainsKey($sectionName)) {
                $document[$sectionName] = @{}
            }
            continue
        }
        $separator = $line.IndexOf("=")
        if ($separator -lt 0) {
            continue
        }
        if (-not $document.ContainsKey($sectionName)) {
            $document[$sectionName] = @{}
        }
        $key = $line.Substring(0, $separator).Trim()
        $value = $line.Substring($separator + 1).Trim()
        $document[$sectionName][$key] = $value
    }

    return $document
}

function Get-ScreenSaveManifestValue {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Manifest,
        [Parameter(Mandatory = $true)]
        [string]$Section,
        [Parameter(Mandatory = $true)]
        [string]$Key
    )

    if (-not $Manifest.ContainsKey($Section)) {
        throw "Missing installer manifest section [$Section]."
    }
    if (-not $Manifest[$Section].ContainsKey($Key)) {
        throw "Missing installer manifest key [$Section] $Key."
    }

    return [string]$Manifest[$Section][$Key]
}

function Get-ScreenSaveDefaultInstallRoot {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Manifest
    )

    $rootSuffix = Get-ScreenSaveManifestValue -Manifest $Manifest -Section "install" -Key "default_root_suffix"
    if ([string]::IsNullOrEmpty($env:LOCALAPPDATA)) {
        throw "LOCALAPPDATA is not defined."
    }
    return Join-Path $env:LOCALAPPDATA $rootSuffix
}

function Get-ScreenSaveMockStateKey {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DesktopValueName
    )

    if ($DesktopValueName -eq "SCRNSAVE.EXE") {
        return "SCRNSAVE_EXE"
    }
    return $DesktopValueName
}

function New-ScreenSaveMockState {
    return @{
        desktop = @{
            SCRNSAVE_EXE = ""
            ScreenSaveActive = "0"
        }
        uninstall = @{}
    }
}

function Get-ScreenSaveMockState {
    param(
        [Parameter(Mandatory = $true)]
        [string]$MockStatePath
    )

    if ([string]::IsNullOrEmpty($MockStatePath)) {
        throw "Mock state path was not supplied."
    }

    if (-not (Test-Path -LiteralPath $MockStatePath)) {
        return New-ScreenSaveMockState
    }

    $raw = Get-Content -LiteralPath $MockStatePath -Raw
    if ([string]::IsNullOrWhiteSpace($raw)) {
        return New-ScreenSaveMockState
    }

    return ConvertTo-ScreenSaveHashtable -InputObject (ConvertFrom-Json -InputObject $raw)
}

function Save-ScreenSaveMockState {
    param(
        [Parameter(Mandatory = $true)]
        [string]$MockStatePath,
        [Parameter(Mandatory = $true)]
        [hashtable]$State
    )

    $directory = Split-Path -Parent $MockStatePath
    if ($directory) {
        New-Item -ItemType Directory -Force -Path $directory | Out-Null
    }
    ($State | ConvertTo-Json -Depth 8) | Set-Content -LiteralPath $MockStatePath -Encoding ASCII
}

function Get-ScreenSaveDesktopValue {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DesktopValueName,
        [string]$MockStatePath = ""
    )

    if (-not [string]::IsNullOrEmpty($MockStatePath)) {
        $state = Get-ScreenSaveMockState -MockStatePath $MockStatePath
        $key = Get-ScreenSaveMockStateKey -DesktopValueName $DesktopValueName
        if ($state.ContainsKey("desktop") -and $state["desktop"].ContainsKey($key)) {
            return [string]$state["desktop"][$key]
        }
        return ""
    }

    $registryPath = "Registry::HKEY_CURRENT_USER\Control Panel\Desktop"
    $item = Get-ItemProperty -LiteralPath $registryPath -ErrorAction SilentlyContinue
    if ($null -eq $item) {
        return ""
    }
    if ($null -eq $item.PSObject.Properties[$DesktopValueName]) {
        return ""
    }
    return [string]$item.$DesktopValueName
}

function Set-ScreenSaveDesktopValue {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DesktopValueName,
        [Parameter(Mandatory = $true)]
        [AllowEmptyString()]
        [string]$Value,
        [string]$MockStatePath = ""
    )

    if (-not [string]::IsNullOrEmpty($MockStatePath)) {
        $state = Get-ScreenSaveMockState -MockStatePath $MockStatePath
        $key = Get-ScreenSaveMockStateKey -DesktopValueName $DesktopValueName
        $state["desktop"][$key] = $Value
        Save-ScreenSaveMockState -MockStatePath $MockStatePath -State $state
        return
    }

    $registryPath = "Registry::HKEY_CURRENT_USER\Control Panel\Desktop"
    Set-ItemProperty -LiteralPath $registryPath -Name $DesktopValueName -Value $Value
}

function Set-ScreenSaveUninstallEntry {
    param(
        [Parameter(Mandatory = $true)]
        [string]$UninstallKey,
        [Parameter(Mandatory = $true)]
        [hashtable]$Values,
        [string]$MockStatePath = ""
    )

    if (-not [string]::IsNullOrEmpty($MockStatePath)) {
        $state = Get-ScreenSaveMockState -MockStatePath $MockStatePath
        $state["uninstall"][$UninstallKey] = $Values
        Save-ScreenSaveMockState -MockStatePath $MockStatePath -State $state
        return
    }

    $registryPath = "Registry::HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\$UninstallKey"
    New-Item -Force -Path $registryPath | Out-Null
    foreach ($name in $Values.Keys) {
        New-ItemProperty -LiteralPath $registryPath -Name $name -Value $Values[$name] -PropertyType String -Force | Out-Null
    }
}

function Remove-ScreenSaveUninstallEntry {
    param(
        [Parameter(Mandatory = $true)]
        [string]$UninstallKey,
        [string]$MockStatePath = ""
    )

    if (-not [string]::IsNullOrEmpty($MockStatePath)) {
        $state = Get-ScreenSaveMockState -MockStatePath $MockStatePath
        if ($state["uninstall"].ContainsKey($UninstallKey)) {
            $state["uninstall"].Remove($UninstallKey) | Out-Null
        }
        Save-ScreenSaveMockState -MockStatePath $MockStatePath -State $state
        return
    }

    $registryPath = "Registry::HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\$UninstallKey"
    if (Test-Path -LiteralPath $registryPath) {
        Remove-Item -LiteralPath $registryPath -Recurse -Force
    }
}

function Write-ScreenSaveJsonFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [hashtable]$Data
    )

    $directory = Split-Path -Parent $Path
    if ($directory) {
        New-Item -ItemType Directory -Force -Path $directory | Out-Null
    }
    ($Data | ConvertTo-Json -Depth 8) | Set-Content -LiteralPath $Path -Encoding ASCII
}

function Read-ScreenSaveJsonFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Missing JSON file: $Path"
    }

    return ConvertTo-ScreenSaveHashtable -InputObject (ConvertFrom-Json -InputObject (Get-Content -LiteralPath $Path -Raw))
}

function Get-ScreenSavePayloadSaverSlugs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$PayloadRoot
    )

    $saversRoot = Join-Path $PayloadRoot "SAVERS"
    if (-not (Test-Path -LiteralPath $saversRoot)) {
        throw "Missing payload saver root: $saversRoot"
    }

    $slugs = @()
    foreach ($file in Get-ChildItem -LiteralPath $saversRoot -Filter "*.scr" -File | Sort-Object Name) {
        $slugs += $file.BaseName
    }
    return $slugs
}

function Copy-ScreenSaveDirectoryContents {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SourceRoot,
        [Parameter(Mandatory = $true)]
        [string]$DestinationRoot
    )

    New-Item -ItemType Directory -Force -Path $DestinationRoot | Out-Null
    foreach ($item in Get-ChildItem -LiteralPath $SourceRoot -Force) {
        Copy-Item -LiteralPath $item.FullName -Destination (Join-Path $DestinationRoot $item.Name) -Recurse -Force
    }
}

function Assert-ScreenSaveInstallTarget {
    param(
        [Parameter(Mandatory = $true)]
        [string]$InstallRoot
    )

    $resolved = [System.IO.Path]::GetFullPath($InstallRoot)
    $trimmed = $resolved.TrimEnd('\')
    $driveRoot = [System.IO.Path]::GetPathRoot($resolved).TrimEnd('\')

    if ([string]::IsNullOrEmpty($trimmed) -or $trimmed -eq $driveRoot) {
        throw "Refusing to use an install root that resolves to a drive root: $resolved"
    }

    return $resolved
}

function Assert-ScreenSaveManagedInstallRoot {
    param(
        [Parameter(Mandatory = $true)]
        [string]$InstallRoot
    )

    $resolved = Assert-ScreenSaveInstallTarget -InstallRoot $InstallRoot
    $statePath = Join-Path $resolved "INSTALLER\install-state.json"

    if (-not (Test-Path -LiteralPath $statePath)) {
        throw "Refusing to remove an install root that does not contain INSTALLER\\install-state.json: $resolved"
    }

    return $resolved
}

function Get-ScreenSaveExistingInstallState {
    param(
        [Parameter(Mandatory = $true)]
        [string]$InstallRoot
    )

    $statePath = Join-Path $InstallRoot "INSTALLER\install-state.json"
    if (Test-Path -LiteralPath $statePath) {
        return Read-ScreenSaveJsonFile -Path $statePath
    }

    return $null
}
