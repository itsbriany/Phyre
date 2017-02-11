Import-Module $env:GAME_ENGINE_ROOT\Build.psm1 -Force -DisableNameChecking

function Install-GameEngine() {
  Init-Submodules
  Set-VS140-Environment
  Prepare-GameEngine-Solution 
  Create-Tools
  Build-Protobuf
  Compile-Protobuf
  Build-Base64
  Build-Gtest
  Build-GameEngine($env:CONFIGURATION)
}

if ($env:APPVEYOR) {
  Install-GameEngine 2> $null
} else {
  Install-GameEngine
}
