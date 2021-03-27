Write-Host "Bitte Visual Studio vorher beenden!"
Read-Host -Prompt "Press any key to continue"

Write-Host "Bearbeitung gestaret..."
Get-ChildItem -Path . -Recurse -Force .vs | Remove-Item -Recurse -Force
Write-Host "Bearbeite Release"
Get-ChildItem -Path Release -Recurse -Force .vs | Remove-Item -Recurse -Force
Write-Host "Bearbeite Debug"
Get-ChildItem -Path Debug -Recurse -Force .vs | Remove-Item -Recurse -Force
Write-Host "Bearbeite CMakeBuilds"
Get-ChildItem -Path .. -Recurse -Force CMakeBuilds | Remove-Item -Recurse -Force
Write-Host "Ferig!"
