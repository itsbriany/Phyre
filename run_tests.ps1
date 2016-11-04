# If you want to filter tests, you can set the gtest_filter
# which will be applied on GameEngineTests.exe.
# Example: .\run_tests.exe -gtest_filter TCP*
# will run all tests starting with TCP.
param (
    [switch]$Release = $false,
    [string]$gtest_filter = ""
)

Import-Module $env:GAME_ENGINE_ROOT\Build.psm1 -Force -DisableNameChecking

$CurrentWorkingDirectory = pwd

# Copy test resources over to the test runtime directories
Copy-Test-Resources

if ($release) {
    cd "$env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Release"
} else {
    cd "$env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Debug"
}

if ($gtest_filter) {
    .\GameEngineTests.exe --gtest_filter=$gtest_filter
} else {
    .\GameEngineTests.exe
}
cd $CurrentWorkingDirectory.Path