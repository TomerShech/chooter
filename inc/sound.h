#ifndef SOUND_H_
#define SOUND_H_

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// The number of sound effects that can be played simultaneously.
#define SOUND_CHANNELS_MAX 8

// Channels.
enum Channel { CHNL_ANY = -1, CHNL_PLAYER, CHNL_ENEMY_BULLET, CHNL_POD };
// Sounds.
enum Sound {
    SND_PLAYER_BULLET,
    SND_ENEMY_BULLET,
    SND_PLAYER_DEATH,
    SND_ENEMY_DEATH,
    SND_POD,
    SND_LEN
};

void music_load(const char *path);
void music_play(bool loop);
void sound_play(enum Sound sound, enum Channel channel);
void sounds_init(void);

#endif // SOUND_H_
