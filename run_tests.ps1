# If you want to filter tests, you can set the gtest_filter
# which will be applied on GameEngineTests.exe.
# Example: .\run_tests.exe -gtest_filter TCP*
# will run all tests starting with TCP.
param (
    [switch]$Release = $false,
    [string]$gtest_filter = ""
)

if ($env:CONFIGURATION -eq "Release") {
    $Release = $true
}

Import-Module $env:PHYRE_ROOT\Build.psm1 -Force -DisableNameChecking

$CurrentWorkingDirectory = Get-Location

if ($release) {
    Set-Location "$env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Release"
    Invoke-Expression "$env:PHYRE_ROOT\Phyre\Build\PhyreTesting\Release\PhyreTests.exe"
} else {
    Set-Location "$env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Debug"
    Invoke-Expression "$env:PHYRE_ROOT\Phyre\Build\PhyreTesting\Debug\PhyreTests.exe"
}



 #   .\PhyreTests.exe --gtest_filter=$gtest_filter

Set-Location $CurrentWorkingDirectory.Path
exit $LASTEXITCODE