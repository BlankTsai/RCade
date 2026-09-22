@echo off
cd /d "%~dp0"
if not exist "profiles" mkdir "profiles"
set "PROFILE_ID=%RANDOM%-%RANDOM%"
"build\music_game.exe" --profile "profiles\quiet-%PROFILE_ID%.csv"
pause
