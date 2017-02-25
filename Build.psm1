function Init-Submodules {
    git submodule update --init --recursive
}

# Copy test resources over to the test runtime directories
function Copy-Test-Environment {
    # We need to dynamically link to the testing libraries
    Set-Location $env:PHYRE_ROOT\Build\Testing
    $debugDir = "Bin\Debug"
    $releaseDir = "Bin\Release"
    mkdir $debugDir -Force
    mkdir $releaseDir -Force
    Copy-Item -Force $env:PHYRE_ROOT\ThirdParty\googletest\Build\googlemock\gtest\Debug\*.dll $debugDir
    Copy-Item -Force $env:PHYRE_ROOT\ThirdParty\googletest\Build\googlemock\gtest\Release\*.dll $releaseDir
    Copy-Item -Force $env:PHYRE_ROOT\ThirdParty\googletest\Build\googlemock\Debug\*.dll $debugDir
    Copy-Item -Force $env:PHYRE_ROOT\ThirdParty\googletest\Build\googlemock\Release\*.dll $releaseDir

    # Some tests require resource files, so we copy them over to each build configuration
    $TestResourceDirectories = Get-ChildItem $env:PHYRE_ROOT\Testing\Resources
    foreach ($TestResourceDirectory in $TestResourceDirectories) {
        Copy-Item -Force -Recurse $TestResourceDirectory.FullName $debugDir
        Copy-Item -Force -Recurse $TestResourceDirectory.FullName $releaseDir
    }
    Set-Location $env:PHYRE_ROOT
}

# Sets Visual Studio 2015 environment variables
function Set-VS140-Environment {
    Push-Location "$env:VS140COMNTOOLS"
    cmd /c "vsvars32.bat&set" |
    ForEach-Object {
        if ($_ -match "=") {
            $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
        }
    }
    Pop-Location
    write-host "`nVisual Studio 2015 Command Prompt variables set." -ForegroundColor Yellow
}

function Prepare-Phyre-Solution {
    Set-Location $env:PHYRE_ROOT
    mkdir Build -Force
    Set-Location Build
    cmake -G "Visual Studio 14 Win64" ..
    Set-Location $env:PHYRE_ROOT
}

function Create-Tools {
    mkdir -Force $env:PHYRE_ROOT\Tools
}

# Build the protobuf compiler
function Build-Protobuf {
    Set-Location $env:PHYRE_ROOT\ThirdParty\protobuf\cmake
    mkdir Build -Force
    Set-Location Build
    cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14 Win64" ..
    cmake --build . --target ALL_BUILD --config Release
    Copy-Item Release\protoc.exe $env:PHYRE_ROOT\Tools
}

# Compile the protobuf files for target platforms
function Compile-Protobuf {
    Set-Location $env:PHYRE_ROOT
    Tools\protoc --proto_path=Phyre\Build\Common\PhyreCommon\ --cpp_out=Phyre\Build\Common Phyre\Build\Common\PhyreCommon\Chat.proto
}

# Build unit testing suite
function Build-Gtest {
    Set-Location $env:PHYRE_ROOT\ThirdParty\googletest
    mkdir Build -Force
    Set-Location Build
    cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14 Win64" ..
    cmake --build . --target ALL_BUILD --config Debug
    cmake --build . --target ALL_BUILD --config Release
}

function Build-Base64 {
    Set-Location $env:PHYRE_ROOT\ThirdParty\libb64-1.2\base64\VisualStudioProject
    msbuild /m base64.sln /p:Configuration=Debug /p:Platform="x64"
    msbuild /m base64.sln /p:Configuration=Release /p:Platform="x64"
    Move-Item -Force $env:PHYRE_ROOT\ThirdParty\libb64-1.2\base64\VisualStudioProject\x64\Release\base64.exe $env:PHYRE_ROOT\Tools
}

function Build-GLFW {
    Set-Location $env:PHYRE_ROOT\ThirdParty\glfw
    mkdir Build -Force
    Set-Location Build
    cmake -G "Visual Studio 14 Win64" ..
    cmake --build . --target ALL_BUILD --config Debug
    cmake --build . --target ALL_BUILD --config Release
}

function Build-Phyre([switch]$Debug, [switch] $Release) {
    Set-Location $env:PHYRE_ROOT\Build
    if (!$Debug -and !$Release) {
        cmake --build . --target ALL_BUILD --config Debug
        cmake --build . --target ALL_BUILD --config Release
    }
    if ($Debug) {
        cmake --build . --target ALL_BUILD --config Debug
    }
    if ($Release) {
        cmake --build . --target ALL_BUILD --config Release
    }
    Set-Location $env:PHYRE_ROOT
}

function Spirv-Compile {
    Set-Location $env:PHYRE_ROOT
    .\spirv_compile.ps1
}
