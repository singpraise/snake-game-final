#ifndef EDITOR_H
#define EDITOR_H

#include "level.h"

typedef enum {
    EDITOR_EXIT_QUIT = 0,
    EDITOR_EXIT_PLAY = 1
} EditorExitCode;

typedef struct {
    EditorExitCode code;
    CustomLevel   *level;
} EditorResult;

typedef enum {
    TOOL_ERASE = 0,
    TOOL_OBSTACLE,
    TOOL_FOOD,
    TOOL_BONUS,
    TOOL_SNAKE
} EditorTool;

EditorResult editor_run_blocking(void);
void         editor_start(void);
void         editor_stop(void);
int          editor_is_active(void);
void         editor_frame(void);
void         editor_render(void);
int          editor_handle_key(int ch);
void         editor_set_status(const char *text);
CustomLevel *editor_take_level(void);
int          editor_play_requested(void);

#endif /* EDITOR_H */
