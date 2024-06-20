#
# Assertions.
#

if ($args.Count -ne 1)
{
    Write-Output "Incorrect number of arguments"
    Exit
}

$BinDir = $args[0]
if (-Not $(Test-Path -Path $BinDir))
{
    Write-Output "Provided directory does not exist"
    Exit
}

#
# Compress sequence.
#

$Destination = $(Split-Path -Path $BinDir -Leaf) + ".zip"
$Destination = $(Join-Path $(Split-Path -Path $BinDir -Parent) $Destination)
$compress = @{
  Path = $BinDir
  CompressionLevel = "Fastest"
  DestinationPath = $Destination
}
Compress-Archive @compress
