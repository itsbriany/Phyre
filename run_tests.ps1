param (
    [switch]$Release = $false
)

if ($env:CONFIGURATION -eq "Release") {
    $Release = $true
}

# Generate xml report for AppVeyor
if ($env:APPVEYOR) {
    $env:GTEST_OUTPUT = "xml"
}

if ($release) {
    Get-ChildItem -Path ThirdParty\googletest\googlemock\Release -Filter *.exe | % {& $_.FullName}
} else {
    Get-ChildItem -Path ThirdParty\googletest\googlemock\Debug -Filter *.exe | % {& $_.FullName}
}

# upload results to AppVeyor
if ($env:APPVEYOR) {
    Resolve-Path $env:GAME_ENGINE_ROOT\test_detail.xml
    $wc = New-Object 'System.Net.WebClient'
    $wc.UploadFile("https://ci.appveyor.com/api/testresults/xunit/$($env:APPVEYOR_JOB_ID)", (Resolve-Path $env:GAME_ENGINE_ROOT\test_detail.xml))
}