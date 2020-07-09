#include "sound.h"

static Mix_Music *music;
static Mix_Chunk *sounds[SND_LEN];

static void load_sounds(void) {
    sounds[SND_PLAYER_BULLET] = Mix_LoadWAV("res/sound/player_bullet.ogg");
    sounds[SND_ENEMY_BULLET] = Mix_LoadWAV("res/sound/enemy_bullet.ogg");
    sounds[SND_PLAYER_DEATH] = Mix_LoadWAV("res/sound/player_death.ogg");
    sounds[SND_ENEMY_DEATH] = Mix_LoadWAV("res/sound/enemy_death.ogg");
    sounds[SND_POD] = Mix_LoadWAV("res/sound/pod.ogg");
}

void music_load(const char *path) {
    if (music) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);

        music = NULL;
    }

    music = Mix_LoadMUS(path);
}

void music_play(bool loop) { Mix_PlayMusic(music, loop ? -1 : 0); }

void sound_play(enum Sound sound, enum Channel channel) {
    Mix_PlayChannel(channel, sounds[sound], 0);
}

void sounds_init(void) {
    music = NULL;

    memset(sounds, 0, sizeof(Mix_Chunk *) * SND_LEN);
    load_sounds();
}
