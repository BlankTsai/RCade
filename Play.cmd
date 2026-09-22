@echo off
cd /d "%~dp0"
"build\music_game.exe"
if errorlevel 1 pause
