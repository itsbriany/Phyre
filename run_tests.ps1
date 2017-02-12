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

# The tests are expected to be built in the googletest build directory so they can
# dynamically link to the gmock and gtest libraries.
if ($release) {
    Set-Location "$env:PHYRE_ROOT\Build\Testing\Bin\Release"
} else {
    Set-Location "$env:PHYRE_ROOT\Build\Testing\Bin\Debug"
}

if ($gtest_filter) {
    .\PhyreTests.exe --gtest_filter=$gtest_filter
} else {
    .\PhyreTests.exe
}
 

Set-Location $CurrentWorkingDirectory.Path
exit $LASTEXITCODE