param (
    [Parameter(Mandatory=$false)][string]$Target = "pid_tests",
    [switch]$Clean = $false
)

$ErrorActionPreference = "Stop"

$preset = "native"
$buildDir = Join-Path $PSScriptRoot "build/$preset"

if ($Clean -and (Test-Path $buildDir))
{
    Write-Host "Removing $buildDir..."
    Remove-Item -Recurse -Force $buildDir
}

Write-Host "Configuring $preset build..."
cmake --preset $preset
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Building $Target..."
cmake --build $buildDir --target $Target
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Running tests..."
ctest --test-dir $buildDir --output-on-failure -R $Target
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
