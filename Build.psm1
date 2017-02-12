function Init-Submodules {
    git submodule update --init --recursive
}

# Copy test resources over to the test runtime directories
# TODO: This is likely to be unnecessary
function Copy-Test-Resources {
    Set-Location $env:PHYRE_ROOT\Phyre
    $TestResourceDirectories = "*TestResources"
    $TestResources = Get-ChildItem -Recurse $TestResourceDirectories | ?{ $_.PSIsContainer }
    foreach ($TestResource in $TestResources) {
        $TestResourceBaseName = $TestResource.BaseName
        $TestResourceFullName = $TestResource.FullName
        mkdir -Force $env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Debug\$TestResourceBaseName > $null
        mkdir -Force $env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Release\$TestResourceBaseName > $null
        Copy-Item -Force $TestResourceFullName\* "$env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Debug\$TestResourceBaseName"
        Copy-Item -Force $TestResourceFullName\* "$env:PHYRE_ROOT\ThirdParty\googletest\googlemock\Release\$TestResourceBaseName"
    }
    Set-Location $env:PHYRE_ROOT
}

# Sets Visual Studio 2015 environment variables
function Set-VS140-Environment {
    Push-Location "$env:VS140COMNTOOLS"
    cmd /c "vsvars32.bat&set" |
    foreach {
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
    cmake -G "Visual Studio 14" ..
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
    cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14" ..
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
    cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14" ..
    cmake --build . --target ALL_BUILD --config Debug
    cmake --build . --target ALL_BUILD --config Release
}

function Build-Base64 {
    Set-Location $env:PHYRE_ROOT\ThirdParty\libb64-1.2\base64\VisualStudioProject
    msbuild /m base64.sln /p:Configuration=Debug /p:Platform="Win32"
    msbuild /m base64.sln /p:Configuration=Release /p:Platform="Win32"
    Move-Item -Force $env:PHYRE_ROOT\ThirdParty\libb64-1.2\base64\VisualStudioProject\Release\base64.exe $env:PHYRE_ROOT\Tools
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
