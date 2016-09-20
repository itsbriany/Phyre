git submodule update --init --recursive

cd %PROTOBUF_ROOT%\cmake
cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14"
msbuild /m protobuf.sln /p:Configuration=Release /p:Platform="Win32"
md %GAME_ENGINE_ROOT%\Tools
xcopy /s Release\protoc.exe %GAME_ENGINE_ROOT%\Tools

cd %GAME_ENGINE_ROOT%
Tools\protoc --proto_path=GameEngine\Common\GameEngineCommon\ --cpp_out=GameEngine\Common GameEngine\Common\GameEngineCommon\Chat.proto

cd %GOOGLE_TEST_DISTRIBUTION%
cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14"
msbuild /m googletest-distribution.sln /p:Configuration=Debug /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1
msbuild /m googletest-distribution.sln /p:Configuration=Release /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1

cd %GAME_ENGINE_ROOT%\GameEngine
cmake -G "Visual Studio 14"