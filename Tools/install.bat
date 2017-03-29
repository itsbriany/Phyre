set
git submodule update --init --recursive
pushd %PHYRE_ROOT%
mkdir Build
pushd Build
cmake -G "Visual Studio 14 Win64" ..
cmake --build . --target ALL_BUILD --config %CONFIGURATION%
popd
popd