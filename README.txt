========================================
  C Snake Game - Windows Guide
========================================

[Project layout]
  types.h      Basic types (Point, Direction)
  memory.h/c   Unified malloc/free (golden rule)
  queue.h/c    Queue body + DirQueue input
  stack.h/c    Stack undo snapshots
  board.h/c    2D array map
  init.h/c     Initialization (pointer + malloc)
  snake.h/c    Game logic
  input_win.c  Windows keyboard input
  main.c       Entry point

[Requirements]
  - Windows 10/11
  - MinGW-w64 gcc (MSYS2 recommended)

[Install compiler (MSYS2)]
  1. Install from https://www.msys2.org/
  2. Open MSYS2 MINGW64 terminal:
     pacman -S mingw-w64-x86_64-gcc
  3. Add to PATH: C:\msys64\mingw64\bin
  4. Restart PowerShell or cmd

[Build and run]
  Option 1: Double-click build.bat, then run.bat
  Option 2: Command line
    cd snake
    build.bat
    snake.exe

[Source encoding]
  All .c / .h files are UTF-8 (no BOM)
  Run convert_utf8.bat to re-convert if needed

[Manual compile]
  gcc -std=c99 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 -o snake.exe ^
      main.c init.c snake.c queue.c stack.c board.c memory.c input_win.c

[Memory debug build]
  gcc -std=c99 -Wall -Wextra -g -DMEM_DEBUG -o snake.exe ^
      main.c init.c snake.c queue.c stack.c board.c memory.c input_win.c

[Controls]
  W / Up     - Move up
  S / Down   - Move down
  A / Left   - Move left
  D / Right  - Move right
  U          - Undo last move
  Q          - Quit

[Symbols]
  @ Head   o Body   * Food   . Empty
