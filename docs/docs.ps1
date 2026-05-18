# PowerShell script to generate Doxygen documentation
$repoRoot = Resolve-Path (Join-Path $PSScriptRoot '..')
$doxyfile = Join-Path $PSScriptRoot 'Doxyfile'
$doxygen = 'doxygen'

if (-not (Get-Command $doxygen -ErrorAction SilentlyContinue)) {
    Write-Host 'Doxygen is not installed or not in PATH.'
    exit 1
}

Write-Host "Generating Doxygen documentation..."
Push-Location $repoRoot
try {
    & $doxygen $doxyfile
}
finally {
    Pop-Location
}
Write-Host "Documentation generated in docs/doxygen/html"
