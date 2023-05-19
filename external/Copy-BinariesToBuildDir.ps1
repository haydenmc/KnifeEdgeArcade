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

Copy-Item -Path "$PSScriptRoot/$arch/$flavor/*.dll" `
    -Destination "$PSScriptRoot/../$BuildDirectory/"
Copy-Item -Path "$PSScriptRoot/$arch/$coreFlavor/*.dll" `
    -Destination "$PSScriptRoot/../$BuildDirectory/"