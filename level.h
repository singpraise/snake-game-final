#ifndef LEVEL_H
#define LEVEL_H

#include "board.h"
#include "types.h"

#define LEVEL_NAME_MAX  64
#define LEVEL_PATH_MAX  260

typedef struct {
    char name[LEVEL_NAME_MAX];
    int width;
    int height;
    int tick_ms;
    int snake_len;
    int snake_x;
    int snake_y;
    Direction snake_dir;
    Cell **cells;
} CustomLevel;

CustomLevel *level_create(int width, int height);
void         level_destroy(CustomLevel *level);
CustomLevel *level_clone(const CustomLevel *level);

Cell  level_get(const CustomLevel *level, int x, int y);
void  level_set(CustomLevel *level, int x, int y, Cell cell);
void  level_clear(CustomLevel *level);

int level_validate(const CustomLevel *level, char *msg, int msg_size);
int level_save(const CustomLevel *level, const char *path);
CustomLevel *level_load(const char *path);
CustomLevel *level_load_string(const char *text);

char level_cell_to_char(Cell cell);
Cell level_char_to_cell(char ch);

#include "snake.h"

GameState *game_create_from_level(const CustomLevel *level);

#endif /* LEVEL_H */
