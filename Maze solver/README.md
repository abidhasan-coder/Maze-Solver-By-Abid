# Maze Solver: DFS (path exploration) and BFS (shortest path)

## Files
- maze_solver.c  : the C program (console)
- maze.txt       : an example maze for "Load maze from file" (menu option 1 -> 5)
- index.html     : web visualizer for the showcase (one file, no install)
- Maze_Solver_Report.docx : project report

## Run the C program
    gcc maze_solver.c -o maze_solver
    ./maze_solver            (Windows: maze_solver.exe)
Menu: 1 choose maze, 2 show maze, 3 DFS, 4 BFS, 5 compare DFS and BFS, 0 exit.
Keep maze.txt in the same folder as the program.

## Deploy the web visualizer (free)
Vercel: put index.html alone in a folder, then drag the folder to vercel.com/new
(or run `npx vercel --prod` inside the folder).
GitHub Pages: upload index.html to a repo, then Settings -> Pages -> deploy from main.
Or just double-click index.html to open it offline.

## Showcase idea (3 minutes)
1. Open the web page: click Run DFS, then Run BFS on the same maze, and compare path lengths.
2. Run the C program: option 5 (compare) on Maze 1: DFS 98 steps vs BFS 32 steps.
3. Choose Maze 4 to show the "no path" case.
