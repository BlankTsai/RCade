@echo off
cd /d "%~dp0"
if not exist "profiles" mkdir "profiles"
set "PROFILE_ID=%RANDOM%-%RANDOM%"
"build\music_game.exe" --verbose --profile "profiles\verbose-%PROFILE_ID%.csv"
pause
