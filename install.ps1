if ($env:APPVEYOR) {
  $env:SUPPRESS_APPVEYOR_ERROR = 2> $null
}

Import-Module $env:GAME_ENGINE_ROOT\Build.psm1 -Force -DisableNameChecking

Init-Submodules
Set-VS140-Environment
Build-Protobuf
Create-Tools
Build-Base64
Compile-Protobuf
Build-Gtest
Copy-Test-Resources
Prepare-GameEngine-Solution