Import-Module -Name ".\Invoke-MsBuild.psm1"

git submodule update --init --recursive

cd $env:PROTOBUF_ROOT\cmake
cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14"
Invoke-MsBuild -Path "protobuf.sln" -Params "/m /p:Configuration=Release /p:Platform=Win32" -ShowBuildOutputInCurrentWindow

# Create Tools directory
md -Force $env:GAME_ENGINE_ROOT\Tools
Copy-Item Release\protoc.exe $env:GAME_ENGINE_ROOT\Tools

cd $env:GAME_ENGINE_ROOT
Tools\protoc --proto_path=GameEngine\Common\GameEngineCommon\ --cpp_out=GameEngine\Common GameEngine\Common\GameEngineCommon\Chat.proto

cd $env:GOOGLE_TEST_DISTRIBUTION
cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14"
Invoke-MsBuild -Path "googletest-distribution.sln" -Params "/m /p:Configuration=Debug /p:Platform=Win32 /p:GTEST_CREATE_SHARED_LIBRARY=1" -ShowBuildOutputInCurrentWindow
Invoke-MsBuild -Path "googletest-distribution.sln" -Params "/m /p:Configuration=Release /p:Platform=Win32 /p:GTEST_CREATE_SHARED_LIBRARY=1" -ShowBuildOutputInCurrentWindow

cd $env:GAME_ENGINE_ROOT\GameEngine
cmake -G "Visual Studio 14"
cd $env:GAME_ENGINE_ROOT