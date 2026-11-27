@echo off
REM Windows: double-click this file to deploy to Vercel
cd /d "%~dp0"
echo Deploying Maze Solver to Vercel...
call npx vercel --prod
echo Done. Copy the Production URL shown above.
pause
