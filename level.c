#include "level.h"
#include "init.h"
#include "memory.h"
#include "score.h"
#include "snake.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

char level_cell_to_char(Cell cell)
{
    return board_cell_to_char(cell);
}

Cell level_char_to_cell(char ch)
{
    switch (ch) {
    case '.': return CELL_EMPTY;
    case '@': return CELL_SNAKE_HEAD;
    case 'o': return CELL_SNAKE;
    case '*': return CELL_FOOD;
    case '$': return CELL_BONUS_FOOD;
    case '#': return CELL_OBSTACLE;
    default:  return CELL_EMPTY;
    }
}

CustomLevel *level_create(int width, int height)
{
    if (width < 5 || height < 5 || width > 40 || height > 30) {
        return NULL;
    }

    CustomLevel *level = (CustomLevel *)mem_calloc(1, sizeof(CustomLevel));
    if (level == NULL) {
        return NULL;
    }

    level->width = width;
    level->height = height;
    level->tick_ms = 110;
    level->snake_len = 3;
    level->snake_x = width / 2;
    level->snake_y = height / 2;
    level->snake_dir = DIR_RIGHT;
    strncpy(level->name, "Untitled", LEVEL_NAME_MAX - 1);

    level->cells = (Cell **)mem_malloc((size_t)height * sizeof(Cell *));
    if (level->cells == NULL) {
        SAFE_FREE(level);
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        level->cells[y] = (Cell *)mem_calloc((size_t)width, sizeof(Cell));
        if (level->cells[y] == NULL) {
            for (int i = 0; i < y; i++) {
                SAFE_FREE(level->cells[i]);
            }
            SAFE_FREE(level->cells);
            SAFE_FREE(level);
            return NULL;
        }
    }

    return level;
}

void level_destroy(CustomLevel *level)
{
    if (level == NULL) {
        return;
    }

    if (level->cells != NULL) {
        for (int y = 0; y < level->height; y++) {
            SAFE_FREE(level->cells[y]);
        }
        SAFE_FREE(level->cells);
    }
    SAFE_FREE(level);
}

CustomLevel *level_clone(const CustomLevel *src)
{
    CustomLevel *copy;

    if (src == NULL) {
        return NULL;
    }

    copy = level_create(src->width, src->height);
    if (copy == NULL) {
        return NULL;
    }

    strncpy(copy->name, src->name, LEVEL_NAME_MAX - 1);
    copy->tick_ms = src->tick_ms;
    copy->snake_len = src->snake_len;
    copy->snake_x = src->snake_x;
    copy->snake_y = src->snake_y;
    copy->snake_dir = src->snake_dir;

    for (int y = 0; y < src->height; y++) {
        memcpy(copy->cells[y], src->cells[y], (size_t)src->width * sizeof(Cell));
    }

    return copy;
}

Cell level_get(const CustomLevel *level, int x, int y)
{
    if (level == NULL || level->cells == NULL) {
        return CELL_EMPTY;
    }
    if (x < 0 || x >= level->width || y < 0 || y >= level->height) {
        return CELL_EMPTY;
    }
    return level->cells[y][x];
}

void level_set(CustomLevel *level, int x, int y, Cell cell)
{
    if (level == NULL || level->cells == NULL) {
        return;
    }
    if (x < 0 || x >= level->width || y < 0 || y >= level->height) {
        return;
    }
    level->cells[y][x] = cell;
}

void level_clear(CustomLevel *level)
{
    if (level == NULL || level->cells == NULL) {
        return;
    }

    for (int y = 0; y < level->height; y++) {
        memset(level->cells[y], 0, (size_t)level->width * sizeof(Cell));
    }
}

static Direction parse_direction(const char *text)
{
    if (text == NULL) {
        return DIR_RIGHT;
    }
    if (strcmp(text, "UP") == 0)    return DIR_UP;
    if (strcmp(text, "DOWN") == 0)  return DIR_DOWN;
    if (strcmp(text, "LEFT") == 0)  return DIR_LEFT;
    return DIR_RIGHT;
}

static const char *direction_to_text(Direction dir)
{
    switch (dir) {
    case DIR_UP:    return "UP";
    case DIR_DOWN:  return "DOWN";
    case DIR_LEFT:  return "LEFT";
    case DIR_RIGHT: return "RIGHT";
    default:        return "RIGHT";
    }
}

int level_validate(const CustomLevel *level, char *msg, int msg_size)
{
    int food_count = 0;
    int head_count = 0;

    if (level == NULL) {
        snprintf(msg, (size_t)msg_size, "Level is NULL");
        return 0;
    }

    if (!snake_body_fits_on_board(level->width, level->height,
                                  level->snake_x, level->snake_y,
                                  level->snake_len, level->snake_dir)) {
        snprintf(msg, (size_t)msg_size, "Snake does not fit on board");
        return 0;
    }

    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            Cell cell = level_get(level, x, y);
            if (cell == CELL_FOOD || cell == CELL_BONUS_FOOD) {
                food_count++;
            }
            if (cell == CELL_SNAKE_HEAD) {
                head_count++;
            }
        }
    }

    if (head_count > 1) {
        snprintf(msg, (size_t)msg_size, "Only one snake head (@) allowed");
        return 0;
    }

    for (int i = 0; i < level->snake_len; i++) {
        Point p;
        switch (level->snake_dir) {
        case DIR_RIGHT: p.x = level->snake_x - i; p.y = level->snake_y; break;
        case DIR_LEFT:  p.x = level->snake_x + i; p.y = level->snake_y; break;
        case DIR_UP:    p.x = level->snake_x; p.y = level->snake_y + i; break;
        case DIR_DOWN:  p.x = level->snake_x; p.y = level->snake_y - i; break;
        default:        p.x = level->snake_x; p.y = level->snake_y; break;
        }

        Cell cell = level_get(level, p.x, p.y);
        if (cell == CELL_OBSTACLE) {
            snprintf(msg, (size_t)msg_size, "Snake overlaps obstacle");
            return 0;
        }
    }

    if (food_count == 0) {
        snprintf(msg, (size_t)msg_size, "Place at least one food (* or $)");
        return 0;
    }

    snprintf(msg, (size_t)msg_size, "OK");
    return 1;
}

int level_save(const CustomLevel *level, const char *path)
{
    FILE *fp;
    int y;

    if (level == NULL || path == NULL) {
        return -1;
    }

    fp = fopen(path, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "# C Snake Level v1\n");
    fprintf(fp, "name=%s\n", level->name);
    fprintf(fp, "width=%d\n", level->width);
    fprintf(fp, "height=%d\n", level->height);
    fprintf(fp, "tick_ms=%d\n", level->tick_ms);
    fprintf(fp, "snake_len=%d\n", level->snake_len);
    fprintf(fp, "snake_x=%d\n", level->snake_x);
    fprintf(fp, "snake_y=%d\n", level->snake_y);
    fprintf(fp, "snake_dir=%s\n", direction_to_text(level->snake_dir));
    fprintf(fp, "---\n");

    for (y = level->height - 1; y >= 0; y--) {
        for (int x = 0; x < level->width; x++) {
            fputc(level_cell_to_char(level_get(level, x, y)), fp);
        }
        fputc('\n', fp);
    }

    fclose(fp);
    return 0;
}

static int level_read_line(const char **cursor, char *line, size_t cap)
{
    const char *p = *cursor;
    size_t i = 0;

    if (p == NULL || *p == '\0') {
        line[0] = '\0';
        return 0;
    }

    while (*p != '\0' && *p != '\n' && *p != '\r' && i + 1 < cap) {
        line[i++] = *p++;
    }
    while (*p == '\n' || *p == '\r') {
        p++;
    }

    line[i] = '\0';
    *cursor = p;
    return (int)i;
}

CustomLevel *level_load_string(const char *text)
{
    const char *cursor = text;
    char line[256];
    CustomLevel *level = NULL;
    int width = 20;
    int height = 15;
    int tick_ms = 110;
    int snake_len = 3;
    int snake_x = 10;
    int snake_y = 7;
    Direction snake_dir = DIR_RIGHT;
    char name[LEVEL_NAME_MAX] = "Loaded";
    int grid_started = 0;
    int grid_y = 0;

    if (text == NULL) {
        return NULL;
    }

    while (level_read_line(&cursor, line, sizeof(line))) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        if (strncmp(line, "name=", 5) == 0) {
            strncpy(name, line + 5, LEVEL_NAME_MAX - 1);
            name[strcspn(name, "\r\n")] = '\0';
        } else if (strncmp(line, "width=", 6) == 0) {
            width = atoi(line + 6);
        } else if (strncmp(line, "height=", 7) == 0) {
            height = atoi(line + 7);
        } else if (strncmp(line, "tick_ms=", 8) == 0) {
            tick_ms = atoi(line + 8);
        } else if (strncmp(line, "snake_len=", 10) == 0) {
            snake_len = atoi(line + 10);
        } else if (strncmp(line, "snake_x=", 8) == 0) {
            snake_x = atoi(line + 8);
        } else if (strncmp(line, "snake_y=", 8) == 0) {
            snake_y = atoi(line + 8);
        } else if (strncmp(line, "snake_dir=", 10) == 0) {
            char dir_buf[16];
            strncpy(dir_buf, line + 10, sizeof(dir_buf) - 1);
            dir_buf[strcspn(dir_buf, "\r\n")] = '\0';
            snake_dir = parse_direction(dir_buf);
        } else if (strncmp(line, "---", 3) == 0) {
            level = level_create(width, height);
            if (level == NULL) {
                return NULL;
            }
            strncpy(level->name, name, LEVEL_NAME_MAX - 1);
            level->tick_ms = tick_ms;
            level->snake_len = snake_len;
            level->snake_x = snake_x;
            level->snake_y = snake_y;
            level->snake_dir = snake_dir;
            grid_started = 1;
            grid_y = height - 1;
        } else if (grid_started && level != NULL && grid_y >= 0) {
            int len = (int)strcspn(line, "\r\n");
            for (int x = 0; x < level->width && x < len; x++) {
                level_set(level, x, grid_y, level_char_to_cell(line[x]));
            }
            grid_y--;
        }
    }

    if (level == NULL) {
        return NULL;
    }

    if (level_get(level, level->snake_x, level->snake_y) != CELL_SNAKE_HEAD) {
        level_set(level, level->snake_x, level->snake_y, CELL_SNAKE_HEAD);
    }

    return level;
}

CustomLevel *level_load(const char *path)
{
    FILE *fp;
    long size;
    char *buf;
    CustomLevel *level;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    buf = (char *)mem_malloc((size_t)size + 1);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }
    if (fread(buf, 1, (size_t)size, fp) != (size_t)size) {
        mem_free(buf);
        fclose(fp);
        return NULL;
    }
    buf[size] = '\0';
    fclose(fp);

    level = level_load_string(buf);
    mem_free(buf);
    return level;
}

GameState *game_create_from_level(const CustomLevel *lvl)
{
    GameState *game;
    char msg[128];
    int has_food = 0;

    if (lvl == NULL || !level_validate(lvl, msg, (int)sizeof(msg))) {
        return NULL;
    }

    game = (GameState *)mem_calloc(1, sizeof(GameState));
    if (game == NULL) {
        return NULL;
    }

    game->width = lvl->width;
    game->height = lvl->height;
    game->length = lvl->snake_len;
    game->dir = lvl->snake_dir;
    game->next_dir = lvl->snake_dir;
    game->mode = MODE_CUSTOM;
    game->difficulty = DIFF_NORMAL;
    game->level = 1;
    game->foods_eaten = 0;
    game->paused = 0;
    game->tick_ms = lvl->tick_ms;

    game->body = snake_body_create(lvl->width, lvl->height,
                                   lvl->snake_x, lvl->snake_y,
                                   lvl->snake_len, lvl->snake_dir);
    if (game->body == NULL) {
        goto fail;
    }

    game->board = board_create(lvl->width, lvl->height);
    if (game->board == NULL) {
        goto fail;
    }

    game->input = dir_queue_create();
    if (game->input == NULL) {
        goto fail;
    }

    game->undo_stack = stack_create();
    if (game->undo_stack == NULL) {
        goto fail;
    }

    for (int y = 0; y < lvl->height; y++) {
        for (int x = 0; x < lvl->width; x++) {
            Cell cell = level_get(lvl, x, y);
            if (cell == CELL_SNAKE || cell == CELL_SNAKE_HEAD) {
                continue;
            }
            board_set(game->board, x, y, cell);
            if (cell == CELL_FOOD || cell == CELL_BONUS_FOOD) {
                game->food.x = x;
                game->food.y = y;
                has_food = 1;
            }
        }
    }

    for (QueueNode *node = game->body->front; node != NULL; node = node->next) {
        Point p = node->data;
        Cell cell = (node == game->body->rear) ? CELL_SNAKE_HEAD : CELL_SNAKE;
        board_set(game->board, p.x, p.y, cell);
    }

    if (!has_food) {
        food_spawn(game);
    }

    game_high_score_load(game);
    srand((unsigned int)time(NULL));
    return game;

fail:
    game_destroy(game);
    return NULL;
}
