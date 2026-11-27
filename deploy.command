#!/bin/bash
# Mac: double-click this file (or run ./deploy.command) to deploy to Vercel
cd "$(dirname "$0")"
echo "Deploying Maze Solver to Vercel..."
npx vercel --prod
echo "Done. Copy the Production URL shown above."
read -p "Press Enter to close"
