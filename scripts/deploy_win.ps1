#
# Assertions.
#

if ($args.Count -ne 2)
{
    Write-Output "Incorrect number of arguments"
    Exit
}

$BuildDir = $args[0]
if (-Not $(Test-Path -Path $BuildDir))
{
    Write-Output "Provided build directory does not exist"
    Exit
}

#
# Deploy sequence.
#

$BinaryName = "cs2guiserver.exe"

$DeployDir = $args[1]
if (-Not $(Test-Path -Path $DeployDir))
{
    New-Item -Name $DeployDir -ItemType "directory"
}

Copy-Item -Path $(Join-Path $BuildDir $BinaryName) -Destination $DeployDir
Copy-Item -Path $(Join-Path $BuildDir "rconpp.dll") -Destination $DeployDir

windeployqt.exe --qmldir $(Join-Path $BuildDir "qml") $(Join-Path $DeployDir $BinaryName)
