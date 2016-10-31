param (
    [switch]$Release = $false
)

# For appveyor CI
if ($env:CONFIGURATION -eq "Release") {
    $Release = $true
}

if ($release) {
    Get-ChildItem -Path ThirdParty\googletest\googlemock\Release -Filter *.exe | % {& $_.FullName}
} else {
    Get-ChildItem -Path ThirdParty\googletest\googlemock\Debug -Filter *.exe | % {& $_.FullName}
}