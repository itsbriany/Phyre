Import-Module $env:PHYRE_ROOT\Build.psm1 -Force -DisableNameChecking

# We may have made some changes to our resources, so we need to copy them over
Copy-Test-Environment

# Configuration is either "Debug" or "Release
$GlslangValidator = "$env:VULKAN_ROOT\glslang\build\StandAlone\Release\glslangValidator.exe"
function spirv-compile ($Configuration) {
    Write-Output "Compiling $Configuration Shaders..."
    $ResourceDir = "$env:PHYRE_ROOT\Build\Testing\Bin\$Configuration\GraphicsTestResources"
    $Resources = Get-ChildItem $ResourceDir
    $SpirvExtension = "spv"
    foreach ($Shader in $Resources) {
        $InputShader = $Shader.FullName
        if ($InputShader -NotLike "*.$SpirvExtension") {
            $BaseName = $Shader.basename
            $Output = "$ResourceDir\$BaseName.$SpirvExtension"
            $command = "$GlslangValidator -V $InputShader -o $Output"
            Invoke-Expression $command
        }
    }
}

spirv-compile("Debug")
spirv-compile("Release")