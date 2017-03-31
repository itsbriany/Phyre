set PATH=C:\Python35;C:\Python35\Scripts;%PATH%
git submodule update --init --recursive

REM cd %PHYRE_ROOT%\ThirdParty\Vulkan-LoaderAndValidationLayers
REM update_external_sources.bat --all
REM build_windows_targets.bat 64

cd %PHYRE_ROOT%
mkdir Build
cd Build
cmake -G "Visual Studio 14 Win64" ..
REM cmake --build . --target ALL_BUILD --config %CONFIGURATION%
cd %PHYRE_ROOT%