#ifndef AUDIO_H
#define AUDIO_H

typedef enum {
    SFX_EAT = 0,
    SFX_BONUS,
    SFX_LEVEL_UP,
    SFX_GAME_OVER,
    SFX_UNDO,
    SFX_PAUSE,
    SFX_EDITOR_PLACE,
    SFX_EDITOR_SAVE,
    SFX_EDITOR_LOAD
} SfxId;

void audio_init(void);
void audio_play(SfxId id);
void audio_set_enabled(int enabled);
int  audio_is_enabled(void);
void audio_toggle(void);

#endif /* AUDIO_H */
