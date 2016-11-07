function Init-Submodules {
    git submodule update --init --recursive
}

# Copy test resources over to the test runtime directories
function Copy-Test-Resources {
    cd $env:GAME_ENGINE_ROOT\GameEngine
    $TestResourceDirectories = "*TestResources"
    $TestResources = Get-ChildItem -Recurse $TestResourceDirectories | ?{ $_.PSIsContainer }
    foreach ($TestResource in $TestResources) {
        $TestResourceBaseName = $TestResource.BaseName
        $TestResourceFullName = $TestResource.FullName
        mkdir -Force $env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Debug\$TestResourceBaseName > $null
        mkdir -Force $env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Release\$TestResourceBaseName > $null
        Copy-Item -Force $TestResourceFullName\* "$env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Debug\$TestResourceBaseName"
        Copy-Item -Force $TestResourceFullName\* "$env:GAME_ENGINE_ROOT\ThirdParty\googletest\googlemock\Release\$TestResourceBaseName"
    }
    cd $env:GAME_ENGINE_ROOT
}

# Sets Visual Studio 2015 environment variables
function Set-VS140-Environment {
    pushd "$env:VS140COMNTOOLS"
    cmd /c "vsvars32.bat&set" |
    foreach {
        if ($_ -match "=") {
            $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
        }
    }
    popd
    write-host "`nVisual Studio 2015 Command Prompt variables set." -ForegroundColor Yellow
}

# Build the protobuf compiler
function Build-Protobuf {
    cd $env:PROTOBUF_ROOT\cmake
    cmake -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14"
    msbuild /m protobuf.sln /p:Configuration=Release /p:Platform="Win32"
}

function Create-Tools {
    mkdir -Force $env:GAME_ENGINE_ROOT\Tools
    Copy-Item Release\protoc.exe $env:GAME_ENGINE_ROOT\Tools
}

# Compile the protobuf files for target platforms
function Compile-Protobuf {
    cd $env:GAME_ENGINE_ROOT
    Tools\protoc --proto_path=GameEngine\Common\GameEngineCommon\ --cpp_out=GameEngine\Common GameEngine\Common\GameEngineCommon\Chat.proto
}

# Build unit testing suite
function Build-Gtest {
    cd $env:GOOGLE_TEST_DISTRIBUTION
    cmake -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -G "Visual Studio 14"
    msbuild /m googletest-distribution.sln /p:Configuration=Debug /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1
    msbuild /m googletest-distribution.sln /p:Configuration=Release /p:Platform="Win32" /p:GTEST_CREATE_SHARED_LIBRARY=1
}

function Build-Base64 {
    cd $env:GAME_ENGINE_ROOT\ThirdParty\libb64-1.2\base64\VisualStudioProject
    msbuild /m base64.sln /p:Configuration=Debug /p:Platform="Win32"
    msbuild /m base64.sln /p:Configuration=Release /p:Platform="Win32"
}

function Prepare-GameEngine-Solution {
    cd $env:GAME_ENGINE_ROOT\GameEngine
    cmake -G "Visual Studio 14"
    cd $env:GAME_ENGINE_ROOT
}

function Build-GameEngine([switch]$Debug, [switch] $Release) {
    cd $env:GAME_ENGINE_ROOT\GameEngine
    if (!$Debug -and !$Release) {
        msbuild /m GameEngine.sln /p:Configuration=Debug /p:Platform="Win32"
        msbuild /m GameEngine.sln /p:Configuration=Release /p:Platform="Win32"
    }

    if ($Debug) {
        msbuild /m GameEngine.sln /p:Configuration=Debug /p:Platform="Win32"
    }
    if ($Release) {
         msbuild /m GameEngine.sln /p:Configuration=Release /p:Platform="Win32"
    }
    cd $env:GAME_ENGINE_ROOT
}
