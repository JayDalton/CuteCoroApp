[CmdletBinding()]
param (
    [Parameter(Mandatory=$false, HelpMessage="Target")]
    [string]$TargetFile = ".\Debug\CuteWidgetApp.exe"
)

$BinaryFile = "$Env:CMAKE_PREFIX_PATH\bin\windeployqt.exe"
if ((Test-Path $BinaryFile) -eq $false) {
    Write-Host "windeployqt.exe nicht gefunden"
    exit
}

if ((Test-Path $TargetFile) -eq $false) {
    Write-Host "Zieldatei (exe) nicht gefunden"
    exit
}

Write-Host "Deploy QT Files" -ForegroundeColor Cyan
& $BinaryFile $TargetFile