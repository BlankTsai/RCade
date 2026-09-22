param([string]$Toolchain = 'C:\msys64\mingw64')
$ErrorActionPreference = 'Stop'
$previousPath = $env:PATH
$env:PATH = "$Toolchain\bin;$env:PATH"
Push-Location $PSScriptRoot
try {
    $compiler = Join-Path $Toolchain 'bin\g++.exe'
    if (!(Test-Path $compiler)) { throw "Compiler not found: $compiler" }
    New-Item -ItemType Directory -Path build -Force | Out-Null
    $sources = @(Get-ChildItem src -Filter '*.cpp' | ForEach-Object FullName)
    & $compiler -std=c++17 -O2 -Wall -Wextra @sources -o build/music_game.exe -I include -I "$Toolchain/include" -L "$Toolchain/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
    if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
    foreach ($dll in @('libraylib.dll','libgcc_s_seh-1.dll','libstdc++-6.dll','libwinpthread-1.dll')) {
        Copy-Item -LiteralPath (Join-Path "$Toolchain/bin" $dll) -Destination build -Force
    }
    Write-Output 'Built build/music_game.exe. Run from the project root.'
} finally { Pop-Location; $env:PATH = $previousPath }
