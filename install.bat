git submodule update --init --recursive

cd %GAME_ENGINE_ROOT%\ThirdParty\protobuf\cmake
cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14"
msbuild /m protobuf.sln /p:Configuration=Release /p:Platform="Win32"
md %GAME_ENGINE_ROOT%\Tools
xcopy /s Release\* %GAME_ENGINE_ROOT%\Tools


cd %GOOGLE_TEST_DISTRIBUTION%
cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14"
msbuild /m googletest-distribution.sln /p:Configuration=Debug /p:Platform="Win32"
msbuild /m googletest-distribution.sln /p:Configuration=Release /p:Platform="Win32"
cd %GAME_ENGINE_ROOT%\GameEngine
md Debug
md Release
xcopy /s %GOOGLE_TEST_DISTRIBUTION%\googlemock\gtest\Debug\* Debug
xcopy /s %GOOGLE_TEST_DISTRIBUTION%\googlemock\gtest\Release\* Release
xcopy /s %GOOGLE_TEST_DISTRIBUTION%\googlemock\Debug\* Debug
xcopy /s %GOOGLE_TEST_DISTRIBUTION%\googlemock\Release\* Release
