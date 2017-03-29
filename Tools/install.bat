set PATH=C:\Python35;C:\Python35\Scripts;%PATH%
git submodule update --init --recursive

REM pushd %PHYRE_ROOT%\ThirdParty\Vulkan-LoaderAndValidationLayers
REM update_external_sources.bat --all
REM build_windows_targets.bat 64 %CONFIGURATION%
REM popd

pushd %PHYRE_ROOT%
dir
mkdir Build
pushd Build
dir
cmake -G "Visual Studio 14 Win64" ..
cmake --build . --target ALL_BUILD --config %CONFIGURATION%
dir
popd
popd