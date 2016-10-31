#Set environment variables for Visual Studio Command Prompt
pushd "$env:VS140COMNTOOLS"
cmd /c "vsvars32.bat&set" |
foreach {
  if ($_ -match "=") {
    $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
  }
}
popd
write-host "`nVisual Studio 2015 Command Prompt variables set." -ForegroundColor Yellow

# Initialize all submodules
git submodule update --init --recursive 2> $null

# Build the protobuf compiler
cd $env:PROTOBUF_ROOT\cmake
cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14" 2> $null
msbuild /m protobuf.sln /p:Configuration=Release /p:Platform="Win32" 2> $null

# Create Tools directory
md -Force $env:GAME_ENGINE_ROOT\Tools
Copy-Item Release\protoc.exe $env:GAME_ENGINE_ROOT\Tools

# Compile the protobuf files for target platforms
cd $env:GAME_ENGINE_ROOT
Tools\protoc --proto_path=GameEngine\Common\GameEngineCommon\ --cpp_out=GameEngine\Common GameEngine\Common\GameEngineCommon\Chat.proto

# Build unit testing suite
cd $env:GOOGLE_TEST_DISTRIBUTION
cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14" 2> $null
msbuild /m googletest-distribution.sln /p:Configuration=Debug /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1 2> $null
msbuild /m googletest-distribution.sln /p:Configuration=Release /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1 2> $null

# Prepare the project solution
cd $env:GAME_ENGINE_ROOT\GameEngine
cmake -G "Visual Studio 14" 2> $null
cd $env:GAME_ENGINE_ROOT