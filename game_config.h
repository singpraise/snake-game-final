#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "types.h"

/* Classic 模式速度（毫秒／幀，越小越快） */
#define TICK_MS_CLASSIC_EASY     150
#define TICK_MS_CLASSIC_NORMAL   120
#define TICK_MS_CLASSIC_HARD      90
#define TICK_MS_CLASSIC_EXTREME   65

/* Enhanced 模式速度 */
#define TICK_MS_ENHANCED_EASY     140
#define TICK_MS_ENHANCED_NORMAL    110
#define TICK_MS_ENHANCED_HARD      85
#define TICK_MS_ENHANCED_EXTREME   58

#define TICK_MS_MIN                42

#define FOODS_PER_LEVEL     3
#define SCORE_NORMAL_FOOD   10
#define SCORE_BONUS_FOOD    30
#define BONUS_FOOD_CHANCE   20

int game_config_initial_obstacles(Difficulty difficulty);
int game_config_obstacles_per_level(Difficulty difficulty);
int game_config_initial_tick_ms(GameMode mode, Difficulty difficulty);
int game_config_tick_for_level(GameMode mode, Difficulty difficulty, int level);

#endif /* GAME_CONFIG_H */
