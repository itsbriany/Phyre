# Run this script in any directory to compile all GLSL .vert and .frag files into SPIR-V bytecode

$GlslangValidator = "$env:VULKAN_SDK\glslang\build\StandAlone\Release\glslangValidator.exe"
Write-Output "Compiling Shaders..."
$SpirvExtension = "spv"
foreach ($File in Get-ChildItem) {
    if ($File -like "*.vert" -Or $File -like "*.frag") {
        $BaseName = $File.basename
        $Output = "$BaseName.$SpirvExtension"
        echo "$File -> $Output"
        $command = "$GlslangValidator -V $File -o $Output"
        Invoke-Expression $command
    }
}