#ifndef EFFECTS_H_
#define EFFECTS_H_

#include <SDL2/SDL.h>

#define STARS_MAX 500

enum { PLAYER, ENEMY, P_BULLET, E_BULLET, BACKGROUND, EXPLOSION, POD, LEN };

typedef struct {
    int speed;
    SDL_Point pos;
} Star;

typedef struct Explosion {
    SDL_FPoint pos;
    SDL_FPoint delta;
    SDL_Color color;
    struct Explosion *next;
} Explosion;

typedef struct Debris {
    int life;
    SDL_FPoint pos;
    SDL_FPoint delta;
    SDL_FRect rect;
    SDL_Texture *tex;
    struct Debris *next;
} Debris;

extern int background_x;
extern Star stars[STARS_MAX];
extern SDL_Texture *textures[LEN];

void stars_init(Star *stars);

#endif // EFFECTS_H_
