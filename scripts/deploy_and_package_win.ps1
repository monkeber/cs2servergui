#
# Assertions.
#

if ($args.Count -ne 2)
{
    Write-Output "Incorrect number of arguments"
    Exit
}

.\scripts\deploy_win.ps1 $args[0] $args[1]
.\scripts\package_win.ps1 $args[1]
