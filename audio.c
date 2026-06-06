#include "audio.h"
#include "platform.h"

static int g_sound_enabled = 1;

void audio_init(void)
{
    platform_sound_init();
}

void audio_play(SfxId id)
{
    if (g_sound_enabled) {
        platform_sound_play(id);
    }
}

void audio_set_enabled(int enabled)
{
    g_sound_enabled = enabled ? 1 : 0;
    platform_sound_set_enabled(g_sound_enabled);
}

int audio_is_enabled(void)
{
    return g_sound_enabled;
}

void audio_toggle(void)
{
    audio_set_enabled(!g_sound_enabled);
}
