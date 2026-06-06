#include "game_config.h"

int game_config_initial_obstacles(Difficulty difficulty)
{
    switch (difficulty) {
    case DIFF_EASY:    return 1;
    case DIFF_NORMAL:  return 2;
    case DIFF_HARD:    return 4;
    case DIFF_EXTREME: return 6;
    default:           return 2;
    }
}

int game_config_obstacles_per_level(Difficulty difficulty)
{
    switch (difficulty) {
    case DIFF_EASY:    return 0;
    case DIFF_NORMAL:  return 1;
    case DIFF_HARD:    return 2;
    case DIFF_EXTREME: return 3;
    default:           return 1;
    }
}

int game_config_initial_tick_ms(GameMode mode, Difficulty difficulty)
{
    if (mode == MODE_CLASSIC) {
        switch (difficulty) {
        case DIFF_EASY:    return TICK_MS_CLASSIC_EASY;
        case DIFF_NORMAL:  return TICK_MS_CLASSIC_NORMAL;
        case DIFF_HARD:    return TICK_MS_CLASSIC_HARD;
        case DIFF_EXTREME: return TICK_MS_CLASSIC_EXTREME;
        default:           return TICK_MS_CLASSIC_NORMAL;
        }
    }

    switch (difficulty) {
    case DIFF_EASY:    return TICK_MS_ENHANCED_EASY;
    case DIFF_NORMAL:  return TICK_MS_ENHANCED_NORMAL;
    case DIFF_HARD:    return TICK_MS_ENHANCED_HARD;
    case DIFF_EXTREME: return TICK_MS_ENHANCED_EXTREME;
    default:           return TICK_MS_ENHANCED_NORMAL;
    }
}

int game_config_tick_for_level(GameMode mode, Difficulty difficulty, int level)
{
    int tick = game_config_initial_tick_ms(mode, difficulty);
    int ramp;

    if (mode == MODE_CLASSIC || mode == MODE_CUSTOM || level <= 1) {
        return tick;
    }

    switch (difficulty) {
    case DIFF_EASY:    ramp = 5; break;
    case DIFF_NORMAL:  ramp = 8; break;
    case DIFF_HARD:    ramp = 10; break;
    case DIFF_EXTREME: ramp = 12; break;
    default:           ramp = 8; break;
    }

    tick -= (level - 1) * ramp;

    if (difficulty >= DIFF_HARD) {
        tick -= (level - 1) * 3;
    }
    if (difficulty == DIFF_EXTREME) {
        tick -= (level - 1) * 2;
    }

    if (tick < TICK_MS_MIN) {
        tick = TICK_MS_MIN;
    }
    return tick;
}
