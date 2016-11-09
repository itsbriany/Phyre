Import-Module $env:GAME_ENGINE_ROOT\Build.psm1 -Force -DisableNameChecking

function Install-GameEngine() {
  Init-Submodules
  Set-VS140-Environment
  Create-Tools
  Build-Base64
  Build-Protobuf
  Compile-Protobuf
  Build-Gtest
  Copy-Test-Resources 
  Prepare-GameEngine-Solution 
}

if ($env:APPVEYOR) {
  Install-GameEngine 2> $null
} else {
  Install-GameEngine
}
