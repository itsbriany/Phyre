set PATH=C:\Python35;C:\Python35\Scripts;%PATH%
git submodule update --init --recursive

pushd %PHYRE_ROOT%\ThirdParty\Vulkan-LoaderAndValidationLayers
update_external_sources.bat --all
build_windows_targets.bat
popd

pushd %PHYRE_ROOT%
mkdir Build
pushd Build
cmake -G "Visual Studio 14 Win64" ..
cmake --build . --target ALL_BUILD --config %CONFIGURATION%
popd
popd