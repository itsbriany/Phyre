Import-Module $env:PHYRE_ROOT\Build.psm1 -Force -DisableNameChecking

function Install-Phyre() {
  Init-Submodules
  Set-VS140-Environment
  Prepare-Phyre-Solution
  Create-Tools
  Build-Base64
  Build-Gtest
  Build-GLFW
  Copy-Test-Environment
  Spirv-Compile
}

if ($env:APPVEYOR) {
  Install-Phyre 2> $null
} else {
  Install-Phyre
}
