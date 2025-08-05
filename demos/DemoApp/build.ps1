 param (
    [Parameter(Mandatory=$true)][string]$qnn_sdk_root = ".",
    [Parameter(Mandatory=$false)][string]$can_build = "true"
 )

function Prepare-App {
    New-Item "$current_dir/build/demo-snapdragon" -ItemType directory -ea 0
    cp "$current_dir/build/src/Release/*" "$current_dir/build/demo-snapdragon/"

    # Copy QNN dependencies
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnHtp.lib" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnHtp.dll" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnHtpPrepare.dll" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnHtpV73Stub.dll" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/hexagon-v73/unsigned/libQnnHtpV73Skel.so" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnHtpNetRunExtensions.dll" "$current_dir/build/demo-snapdragon/"
    cp "$qnn_sdk_root/lib/arm64x-windows-msvc/QnnSystem.dll" "$current_dir/build/demo-snapdragon/"
}

try {
    $ErrorActionPreference = "Stop"
    $initial_dir = (Get-Item .).FullName
    $current_dir = $PSScriptRoot
    New-Item "$current_dir/build" -ItemType directory -ea 0
    Set-Location -Path "$current_dir/build"
    cp "$current_dir/vcpkg.json" "$current_dir/build/"
    cp "$current_dir/vcpkg-configuration.json" "$current_dir/build/"

    $vcpkg_root = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath("C:\vcpkg")
    $qnn_sdk_root = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($qnn_sdk_root)

    write-output "vcpkg root: " $vcpkg_root
    write-output "QNN SDK root : " $qnn_sdk_root

    If (-not (Test-Path $vcpkg_root)){
        git clone https://github.com/microsoft/vcpkg $vcpkg_root
    }
    cd "$current_dir/build"
    C:\vcpkg\bootstrap-vcpkg.bat
    C:\vcpkg\vcpkg integrate install

    $env:QNN_SDK_ROOT = "$qnn_sdk_root"
    cmake -G "Visual Studio 17 2022" -A ARM64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_root/scripts/buildsystems/vcpkg.cmake" ..

    If ($can_build -eq "true") {
        cmake --build . --config Release

        Prepare-App
    }
}
finally {
    Set-Location -Path "$initial_dir"
}
