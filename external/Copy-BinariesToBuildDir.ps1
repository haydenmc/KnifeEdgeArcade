# Dirty script to copy mupen64plus binaries to the project's build directory
# until I spend more time fixing the build system.
param(
    [Parameter()]
    [string]
    $BuildDirectory = "builddir"
)

$arch = "x64"
$flavor = "Debug"
$coreFlavor = "New_Dynarec_Debug"

Copy-Item -Path (`
    "$PSScriptRoot/$arch/$flavor/*.dll",`
    "$PSScriptRoot/$arch/$coreFlavor/*.dll",`
    "$PSScriptRoot/$arch/$flavor/Glide64mk2.ini",`
    "$PSScriptRoot/$arch/$flavor/InputAutoCfg.ini") `
    -Destination "$PSScriptRoot/../$BuildDirectory/"