/*
 * ============================================================
 *  Project : Maze Solver
 *            DFS for path exploration, BFS for shortest path
 *            in an unweighted maze
 *  Language: C (standard C, console based)
 *  Course  : Algorithms Lab
 * ============================================================
 *  How to compile :  gcc maze_solver.c -o maze_solver
 *  How to run     :  ./maze_solver        (Windows: maze_solver.exe)
 *
 *  Maze symbols:  #  = wall      (space) = open path
 *                 S  = start     E = end
 *  Output symbols: *  = final path    .  = cell visited by the algorithm
 * ============================================================
 */

#include <stdio.h>
#include <string.h>

#define MAX_R 25          /* maximum rows    */
#define MAX_C 60          /* maximum columns */

/* ---------- Global data (kept global to keep code simple) ---------- */
char maze[MAX_R][MAX_C + 1];   /* the maze itself                       */
int  visited[MAX_R][MAX_C];    /* 1 if the algorithm visited the cell   */
int  onPath[MAX_R][MAX_C];     /* 1 if the cell is on the final path    */
int  parentR[MAX_R][MAX_C];    /* parent row of each cell (for path)    */
int  parentC[MAX_R][MAX_C];    /* parent column of each cell            */

int rows = 0, cols = 0;        /* size of the current maze              */
int startR, startC;            /* position of S                         */
int endR, endC;                /* position of E                         */
int mazeLoaded = 0;            /* 0 = no maze yet, 1 = maze ready       */

/* Four directions: Up, Right, Down, Left */
int dr[4] = {-1, 0, 1, 0};
int dc[4] = { 0, 1, 0, -1};

/* Result of the last search (used for the comparison table) */
int exploredCount = 0;         /* how many cells were visited           */
int pathLength    = 0;         /* number of steps from S to E           */

/* ---------- Built-in sample mazes ---------- */
static const char *maze1[] = {
    "#########################",
    "#S#     #           #   #",
    "# # ### ##### ###   ### #",
    "# #   # #   #     #     #",
    "# # # # # # # ######### #",
    "# #   # # #   #         #",
    "# ### # # ##### #########",
    "# #   # # #     #       #",
    "# # ### #   ##########  #",
    "#         #            E#",
    "#########################",
};

static const char *maze2[] = {
    "###############################",
    "#S  #           #     #       #",
    "### # # ##### # # # # #   ### #",
    "# #   #       # # # # # # # # #",
    "# ############# ### # ### # # #",
    "#     #       #     #     #   #",
    "# ### # # # ####### # # ##### #",
    "# #       # #     #         # #",
    "# #########   #  ## ####### # #",
    "# #       # # #   #           #",
    "# # ##### # # ### # ##### # ###",
    "# # #   #     # #         #   #",
    "# # ### ####### ############# #",
    "#                            E#",
    "###############################",
};

static const char *maze3[] = {
    "#########################",
    "#S      #         #     #",
    "####### # ####### # #####",
    "#     # #     #   #     #",
    "# # # # ##### # ####### #",
    "# # # # #     #     #   #",
    "# # ### # ######### # # #",
    "# #   #   #   #   # # # #",
    "# ### ##### ### # # # # #",
    "#   #           #     #E#",
    "#########################",
};

static const char *maze4[] = {
    "#########################",
    "#S      #         #     #",
    "####### # ####### # #####",
    "#     # #     #   #     #",
    "# # # # ##### # ####### #",
    "# # # # #     #     #   #",
    "# # ### # ######### # # #",
    "# #   #   #   #   # # # #",
    "# ### ##### ### # # # ###",
    "#   #           #     #E#",
    "#########################",
};

/* ============================================================
 *  PART 1 : Loading and printing the maze
 * ============================================================ */

/* Copy one of the built-in mazes into the 'maze' array */
void loadSample(const char *src[], int r) {
    int i;
    rows = r;
    cols = (int)strlen(src[0]);
    for (i = 0; i < rows; i++) {
        strcpy(maze[i], src[i]);
    }
}

/* Find the positions of S and E. Returns 1 if both exist, else 0 */
int findStartEnd(void) {
    int i, j, foundS = 0, foundE = 0;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (maze[i][j] == 'S') { startR = i; startC = j; foundS = 1; }
            if (maze[i][j] == 'E') { endR = i;   endC = j;   foundE = 1; }
        }
    }
    return foundS && foundE;
}

/* Load a maze from a text file (each line is one row of the maze) */
int loadFromFile(const char *fileName) {
    FILE *fp = fopen(fileName, "r");
    char line[256];
    int i, len, maxLen = 0;

    if (fp == NULL) {
        printf("  Could not open file '%s'.\n", fileName);
        return 0;
    }
    rows = 0;
    while (rows < MAX_R && fgets(line, sizeof(line), fp) != NULL) {
        len = (int)strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';           /* remove the new-line character */
        }
        if (len == 0) continue;           /* skip empty lines              */
        if (len > MAX_C) len = MAX_C;     /* limit the width               */
        strncpy(maze[rows], line, len);
        maze[rows][len] = '\0';
        if (len > maxLen) maxLen = len;
        rows++;
    }
    fclose(fp);
    cols = maxLen;

    /* make every row the same width (fill missing cells with walls) */
    for (i = 0; i < rows; i++) {
        len = (int)strlen(maze[i]);
        while (len < cols) maze[i][len++] = '#';
        maze[i][cols] = '\0';
    }
    if (rows == 0 || !findStartEnd()) {
        printf("  The file must contain one S and one E.\n");
        return 0;
    }
    return 1;
}

/* Clear the visited / path / parent arrays before every search */
void resetSearch(void) {
    int i, j;
    for (i = 0; i < MAX_R; i++) {
        for (j = 0; j < MAX_C; j++) {
            visited[i][j] = 0;
            onPath[i][j]  = 0;
            parentR[i][j] = -1;
            parentC[i][j] = -1;
        }
    }
    exploredCount = 0;
    pathLength    = 0;
}

/* Print the maze. mode 0 = plain maze, mode 1 = show visited + path */
void printMaze(int mode) {
    int i, j;
    char ch;
    printf("\n");
    for (i = 0; i < rows; i++) {
        printf("    ");
        for (j = 0; j < cols; j++) {
            ch = maze[i][j];
            if (mode == 1 && ch != 'S' && ch != 'E' && ch != '#') {
                if (onPath[i][j])       ch = '*';
                else if (visited[i][j]) ch = '.';
            }
            putchar(ch);
        }
        putchar('\n');
    }
    printf("\n");
}

/* Can we walk on cell (r, c)?  It must be inside the maze and not a wall */
int isOpen(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return 0;
    return maze[r][c] != '#';
}

/* ============================================================
 *  PART 2 : Path reconstruction (used by both DFS and BFS)
 *  We start at E and keep moving to the parent until we reach S.
 * ============================================================ */
void buildPath(void) {
    int r = endR, c = endC, pr, pc;
    pathLength = 0;
    while (!(r == startR && c == startC)) {
        onPath[r][c] = 1;
        pr = parentR[r][c];
        pc = parentC[r][c];
        r = pr;
        c = pc;
        pathLength++;
    }
    onPath[startR][startC] = 1;
}

/* ============================================================
 *  PART 3 : DFS - Depth First Search (recursive)
 *  Goes as deep as possible in one direction, then backtracks.
 *  It finds A path, but not always the shortest one.
 * ============================================================ */
int dfs(int r, int c) {
    int d, nr, nc;

    visited[r][c] = 1;
    exploredCount++;

    if (r == endR && c == endC) return 1;       /* reached the end */

    for (d = 0; d < 4; d++) {                   /* Up, Right, Down, Left */
        nr = r + dr[d];
        nc = c + dc[d];
        if (isOpen(nr, nc) && !visited[nr][nc]) {
            parentR[nr][nc] = r;                /* remember where we came from */
            parentC[nr][nc] = c;
            if (dfs(nr, nc)) return 1;          /* stop when the end is found  */
        }
    }
    return 0;                                   /* dead end -> backtrack */
}

int runDFS(void) {
    int found;
    resetSearch();
    found = dfs(startR, startC);
    if (found) buildPath();
    return found;
}

/* ============================================================
 *  PART 4 : BFS - Breadth First Search (queue)
 *  Visits cells level by level (distance 1, 2, 3 ...).
 *  In an unweighted maze the first time we reach E is the
 *  SHORTEST path.
 * ============================================================ */
int queueR[MAX_R * MAX_C];
int queueC[MAX_R * MAX_C];

int runBFS(void) {
    int front = 0, rear = 0;
    int r, c, d, nr, nc;

    resetSearch();
    visited[startR][startC] = 1;
    queueR[rear] = startR;
    queueC[rear] = startC;
    rear++;

    while (front < rear) {
        r = queueR[front];                      /* take one cell out */
        c = queueC[front];
        front++;
        exploredCount++;

        if (r == endR && c == endC) {           /* reached the end */
            buildPath();
            return 1;
        }
        for (d = 0; d < 4; d++) {
            nr = r + dr[d];
            nc = c + dc[d];
            if (isOpen(nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = 1;
                parentR[nr][nc] = r;
                parentC[nr][nc] = c;
                queueR[rear] = nr;              /* put neighbour in queue */
                queueC[rear] = nc;
                rear++;
            }
        }
    }
    return 0;                                   /* queue empty -> no path */
}

/* ============================================================
 *  PART 5 : Menu actions
 * ============================================================ */
void showResult(const char *name, int found) {
    printMaze(1);
    printf("  Algorithm        : %s\n", name);
    if (found) {
        printf("  Path found       : YES\n");
        printf("  Cells explored   : %d\n", exploredCount);
        printf("  Path length      : %d steps\n", pathLength);
    } else {
        printf("  Path found       : NO (the end cannot be reached)\n");
        printf("  Cells explored   : %d\n", exploredCount);
    }
    printf("  Legend: # wall, * path, . visited, S start, E end\n");
}

void compareBoth(void) {
    int f1, f2, e1, e2, p1, p2;

    f1 = runDFS(); e1 = exploredCount; p1 = pathLength;
    f2 = runBFS(); e2 = exploredCount; p2 = pathLength;

    printf("\n  ----------- COMPARISON: DFS vs BFS -----------\n");
    printf("  %-22s %-12s %-12s\n", "", "DFS", "BFS");
    printf("  %-22s %-12s %-12s\n", "Path found", f1 ? "Yes" : "No", f2 ? "Yes" : "No");
    if (f1 && f2) {
        printf("  %-22s %-12d %-12d\n", "Path length (steps)", p1, p2);
    }
    printf("  %-22s %-12d %-12d\n", "Cells explored", e1, e2);
    if (f1 && f2) {
        if (p1 > p2)
            printf("\n  BFS found a shorter path (%d steps shorter).\n", p1 - p2);
        else
            printf("\n  Both algorithms found a path of the same length.\n");
    }
    printf("  -----------------------------------------------\n");
}

void chooseMaze(void) {
    int choice;
    printf("\n  Choose a maze:\n");
    printf("   1. Maze 1 - Small maze with loops\n");
    printf("   2. Maze 2 - Medium maze with many loops\n");
    printf("   3. Maze 3 - Perfect maze (one path only)\n");
    printf("   4. Maze 4 - Blocked maze (no path)\n");
    printf("   5. Load maze from file (maze.txt)\n");
    printf("  Enter choice: ");
    if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); return; }

    switch (choice) {
        case 1: loadSample(maze1, 11); break;
        case 2: loadSample(maze2, 15); break;
        case 3: loadSample(maze3, 11); break;
        case 4: loadSample(maze4, 11); break;
        case 5:
            if (!loadFromFile("maze.txt")) return;
            break;
        default:
            printf("  Invalid choice.\n");
            return;
    }
    if (!findStartEnd()) {
        printf("  This maze has no S or E.\n");
        return;
    }
    mazeLoaded = 1;
    resetSearch();
    printf("\n  Maze loaded (%d rows x %d columns).", rows, cols);
    printMaze(0);
}

int main(void) {
    int choice;

    printf("==============================================================\n");
    printf("   MAZE SOLVER - DFS (path exploration) & BFS (shortest path)\n");
    printf("==============================================================\n");

    loadSample(maze1, 11);      /* start with Maze 1 */
    findStartEnd();
    mazeLoaded = 1;
    resetSearch();

    do {
        printf("\n-------------------- MAIN MENU --------------------\n");
        printf("  1. Choose a maze\n");
        printf("  2. Show current maze\n");
        printf("  3. Solve with DFS (explore a path)\n");
        printf("  4. Solve with BFS (shortest path)\n");
        printf("  5. Compare DFS and BFS\n");
        printf("  0. Exit\n");
        printf("  Enter your choice: ");

        if (scanf("%d", &choice) != 1) {        /* user typed a letter */
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: chooseMaze(); break;
            case 2: printMaze(0); break;
            case 3: showResult("DFS (Depth First Search)", runDFS()); break;
            case 4: showResult("BFS (Breadth First Search)", runBFS()); break;
            case 5: compareBoth(); break;
            case 0: printf("\n  Thank you! Goodbye.\n"); break;
            default: printf("  Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}
