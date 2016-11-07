clang++ --version

# Generate necessary directories
mkdir $GAME_ENGINE_ROOT/Tools

# Build base64 library
cd $GAME_ENGINE_ROOT/ThirdParty/libb64-1.2
make
mv base64/base64 $GAME_ENGINE_ROOT/Tools

# Build protobuf library
cd $GAME_ENGINE_ROOT/ThirdParty/protobuf/cmake
cmake -Dprotobuf_BUILD_TESTS=OFF
make
mv protoc $GAME_ENGINE_ROOT/Tools

# Compile protobuf files
cd $GAME_ENGINE_ROOT
./Tools/protoc --proto_path=GameEngine/Common/GameEngineCommon --cpp_out=GameEngine/Common GameEngine/Common/GameEngineCommon/Chat.proto

# Build gtest distribution
cd $GAME_ENGINE_ROOT/ThirdParty/googletest
cmake -DBUILD_GMOCK:BOOL=ON
make

# Build GameEngine
cd $GAME_ENGINE_ROOT/GameEngine
cmake CMakeLists.txt
make
