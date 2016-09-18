cd ThirdParty/protobuf/cmake
cmake -Dprotobuf_BUILD_TESTS=OFF
make
mv protoc $GAME_ENGINE_ROOT/Tools

# Compile protobuf files
cd $GAME_ENGINE_ROOT
./Tools/protoc --proto_path=GameEngine/Common/GameEngineCommon --cpp_out=GameEngine/Common GameEngine/Common/GameEngineCommon/Chat.proto

cd $GOOGLE_TEST_DISTRIBUTION
cmake -DBUILD_GMOCK:BOOL=ON
make

cd $GAME_ENGINE_ROOT/GameEngine
cmake
make
