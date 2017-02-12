clang++ --version

# Generate necessary directories
mkdir $PHYRE_ROOT/Tools

# Build base64 library
cd $PHYRE_ROOT/ThirdParty/libb64-1.2
make
mv base64/base64 $PHYRE_ROOT/Tools

# Build protobuf library
cd $PHYRE_ROOT/ThirdParty/protobuf/cmake
cmake -Dprotobuf_BUILD_TESTS=OFF
make
mv protoc $PHYRE_ROOT/Tools

# Compile protobuf files
cd $PHYRE_ROOT
./Tools/protoc --proto_path=Phyre/Common/PhyreCommon --cpp_out=Phyre/Common Phyre/Common/PhyreCommon/Chat.proto

# Build gtest distribution
cd $PHYRE_ROOT/ThirdParty/googletest
cmake -DBUILD_GMOCK:BOOL=ON
make

# Build Phyre
cd $PHYRE_ROOT/Phyre
cmake CMakeLists.txt
make
