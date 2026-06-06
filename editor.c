#include "editor.h"
#include "audio.h"
#include "platform.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#endif

#define EDITOR_SAVE_PATH "levels/last.lvl"
#define EDITOR_EXAMPLE_PATH "levels/example.lvl"

typedef struct {
    CustomLevel *level;
    int cursor_x;
    int cursor_y;
    EditorTool tool;
    char status[96];
    int active;
    int play_requested;
} EditorState;

static EditorState g_editor;

static const char *tool_name(EditorTool tool)
{
    switch (tool) {
    case TOOL_ERASE:    return "Erase (.)";
    case TOOL_OBSTACLE: return "Obstacle (#)";
    case TOOL_FOOD:     return "Food (*)";
    case TOOL_BONUS:    return "Bonus ($)";
    case TOOL_SNAKE:    return "Snake (@)";
    default:            return "?";
    }
}

static Cell tool_to_cell(EditorTool tool)
{
    switch (tool) {
    case TOOL_ERASE:    return CELL_EMPTY;
    case TOOL_OBSTACLE: return CELL_OBSTACLE;
    case TOOL_FOOD:     return CELL_FOOD;
    case TOOL_BONUS:    return CELL_BONUS_FOOD;
    case TOOL_SNAKE:    return CELL_SNAKE_HEAD;
    default:            return CELL_EMPTY;
    }
}

static const char *dir_name(Direction dir)
{
    switch (dir) {
    case DIR_UP:    return "UP";
    case DIR_DOWN:  return "DOWN";
    case DIR_LEFT:  return "LEFT";
    case DIR_RIGHT: return "RIGHT";
    default:        return "RIGHT";
    }
}

void editor_set_status(const char *text)
{
    strncpy(g_editor.status, text, sizeof(g_editor.status) - 1);
    g_editor.status[sizeof(g_editor.status) - 1] = '\0';
}

static void editor_clear_snake_markers(void)
{
    CustomLevel *level = g_editor.level;
    int x;
    int y;

    if (level == NULL) {
        return;
    }

    for (y = 0; y < level->height; y++) {
        for (x = 0; x < level->width; x++) {
            Cell cell = level_get(level, x, y);
            if (cell == CELL_SNAKE_HEAD || cell == CELL_SNAKE) {
                level_set(level, x, y, CELL_EMPTY);
            }
        }
    }
}

static void editor_move_cursor(int dx, int dy)
{
    CustomLevel *level = g_editor.level;

    if (level == NULL) {
        return;
    }

    g_editor.cursor_x += dx;
    g_editor.cursor_y += dy;

    if (g_editor.cursor_x < 0) {
        g_editor.cursor_x = 0;
    }
    if (g_editor.cursor_y < 0) {
        g_editor.cursor_y = 0;
    }
    if (g_editor.cursor_x >= level->width) {
        g_editor.cursor_x = level->width - 1;
    }
    if (g_editor.cursor_y >= level->height) {
        g_editor.cursor_y = level->height - 1;
    }
}

static void editor_place_tool(void)
{
    CustomLevel *level = g_editor.level;

    if (level == NULL) {
        return;
    }

    if (g_editor.tool == TOOL_SNAKE) {
        editor_clear_snake_markers();
        level->snake_x = g_editor.cursor_x;
        level->snake_y = g_editor.cursor_y;
        level_set(level, g_editor.cursor_x, g_editor.cursor_y, CELL_SNAKE_HEAD);
    } else {
        level_set(level, g_editor.cursor_x, g_editor.cursor_y, tool_to_cell(g_editor.tool));
    }

    audio_play(SFX_EDITOR_PLACE);
    editor_set_status("Placed on board");
}

static void editor_cycle_dir(void)
{
    if (g_editor.level == NULL) {
        return;
    }

    g_editor.level->snake_dir = (Direction)(((int)g_editor.level->snake_dir + 1) % 4);
    editor_set_status("Snake direction changed");
}

void editor_render(void)
{
    CustomLevel *level = g_editor.level;
    int y;
    int x;

    if (level == NULL) {
        return;
    }

    platform_clear_screen();
    printf("=== Level Editor ===\n");
    printf("Tool: %s | Snake dir: %s | Sound: %s\n",
           tool_name(g_editor.tool), dir_name(level->snake_dir),
           audio_is_enabled() ? "ON" : "OFF");
    printf("Status: %s\n", g_editor.status);
    printf("1-5 tool | W/X/A/D move | Space place | H dir | S save | L load | T test | M mute | Q quit\n\n");

    for (y = level->height - 1; y >= 0; y--) {
        for (x = 0; x < level->width; x++) {
            if (x == g_editor.cursor_x && y == g_editor.cursor_y) {
                putchar('+');
            } else {
                putchar(level_cell_to_char(level_get(level, x, y)));
            }
        }
        putchar('\n');
    }
}

int editor_handle_key(int ch)
{
    CustomLevel *level = g_editor.level;
    char msg[96];

    if (level == NULL) {
        return EDITOR_EXIT_QUIT;
    }

    switch (ch) {
    case 'q': case 'Q':
        return EDITOR_EXIT_QUIT;
    case 't': case 'T':
        if (!level_validate(level, msg, (int)sizeof(msg))) {
            editor_set_status(msg);
            return -1;
        }
        g_editor.play_requested = 1;
        return EDITOR_EXIT_PLAY;
    case 's': case 'S':
        if (level_save(level, EDITOR_SAVE_PATH) == 0) {
            audio_play(SFX_EDITOR_SAVE);
            editor_set_status("Saved levels/last.lvl");
        } else {
            editor_set_status("Save failed");
        }
        return -1;
    case 'l': case 'L': {
        CustomLevel *loaded = level_load(EDITOR_EXAMPLE_PATH);
        if (loaded == NULL) {
            loaded = level_load(EDITOR_SAVE_PATH);
        }
        if (loaded != NULL) {
            level_destroy(level);
            g_editor.level = loaded;
            g_editor.cursor_x = loaded->width / 2;
            g_editor.cursor_y = loaded->height / 2;
            audio_play(SFX_EDITOR_LOAD);
            editor_set_status("Level loaded");
        } else {
            editor_set_status("Load failed");
        }
        return -1;
    }
    case 'h': case 'H':
        editor_cycle_dir();
        return -1;
    case 'm': case 'M':
        audio_toggle();
        return -1;
    case ' ':
        editor_place_tool();
        return -1;
    case '1': g_editor.tool = TOOL_ERASE; return -1;
    case '2': g_editor.tool = TOOL_OBSTACLE; return -1;
    case '3': g_editor.tool = TOOL_FOOD; return -1;
    case '4': g_editor.tool = TOOL_BONUS; return -1;
    case '5': g_editor.tool = TOOL_SNAKE; return -1;
    case 'w': case 'W': editor_move_cursor(0, 1); return -1;
    case 'x': case 'X': editor_move_cursor(0, -1); return -1;
    case 'a': case 'A': editor_move_cursor(-1, 0); return -1;
    case 'd': case 'D': editor_move_cursor(1, 0); return -1;
    case 0x148: editor_move_cursor(0, 1); return -1;
    case 0x150: editor_move_cursor(0, -1); return -1;
    case 0x14B: editor_move_cursor(-1, 0); return -1;
    case 0x14D: editor_move_cursor(1, 0); return -1;
    default:
        return -1;
    }
}

#ifdef _WIN32
static int editor_read_key(void)
{
    int ch = _getch();
    if (ch == 224 || ch == 0) {
        return _getch() | 0x100;
    }
    return ch;
}
#endif

void editor_start(void)
{
    memset(&g_editor, 0, sizeof(g_editor));
    g_editor.level = level_load(EDITOR_EXAMPLE_PATH);
    if (g_editor.level == NULL) {
        g_editor.level = level_create(20, 15);
        if (g_editor.level != NULL) {
            g_editor.level->snake_x = 10;
            g_editor.level->snake_y = 7;
            level_set(g_editor.level, 10, 7, CELL_SNAKE_HEAD);
            level_set(g_editor.level, 14, 7, CELL_FOOD);
            level_set(g_editor.level, 5, 10, CELL_OBSTACLE);
            level_set(g_editor.level, 6, 10, CELL_OBSTACLE);
        }
    }
    g_editor.cursor_x = g_editor.level ? g_editor.level->width / 2 : 0;
    g_editor.cursor_y = g_editor.level ? g_editor.level->height / 2 : 0;
    g_editor.tool = TOOL_OBSTACLE;
    g_editor.active = 1;
    editor_set_status("Welcome to the level editor");
}

void editor_stop(void)
{
    level_destroy(g_editor.level);
    memset(&g_editor, 0, sizeof(g_editor));
}

int editor_is_active(void)
{
    return g_editor.active && g_editor.level != NULL;
}

int editor_play_requested(void)
{
    return g_editor.play_requested;
}

CustomLevel *editor_take_level(void)
{
    CustomLevel *copy = level_clone(g_editor.level);
    editor_stop();
    return copy;
}

void editor_frame(void)
{
    if (!editor_is_active()) {
        return;
    }

    editor_render();

#ifdef _WIN32
    if (_kbhit()) {
        int result = editor_handle_key(editor_read_key());
        if (result == EDITOR_EXIT_QUIT) {
            editor_stop();
        } else if (result == EDITOR_EXIT_PLAY) {
            /* caller should use editor_take_level() */
        }
    }
#endif
}

EditorResult editor_run_blocking(void)
{
    EditorResult out = { EDITOR_EXIT_QUIT, NULL };
    int running = 1;

    editor_start();

    while (running && editor_is_active()) {
        editor_render();

#ifndef _WIN32
        editor_set_status("Editor input needs Windows console");
        break;
#else
        {
            int result = editor_handle_key(editor_read_key());
            if (result == EDITOR_EXIT_QUIT) {
                running = 0;
                out.code = EDITOR_EXIT_QUIT;
            } else if (result == EDITOR_EXIT_PLAY) {
                running = 0;
                out.code = EDITOR_EXIT_PLAY;
                out.level = editor_take_level();
                return out;
            }
        }
#endif
    }

    editor_stop();
    return out;
}
