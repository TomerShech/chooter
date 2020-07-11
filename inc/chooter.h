#ifndef CHOOTER_H_
#define CHOOTER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "effects.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define GLYPH_WIDTH 18
#define GLYPH_HEIGHT 28

#define FPS 60

#define LINE_LEN_MAX 1024
#define TEX_NAME_LEN_MAX 64 // Just a reasonable file name limit, could be changed if needed.

#define PLAYER_SPD 6
#define PLAYER_BULLET_SPD 20
#define ENEMY_BULLET_SPD 8

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct State State;
typedef struct Arena Arena;

typedef void (*delegate_fn)(State *, Arena *);

enum GameKey {
    GK_UP = 1 << 0,
    GK_DOWN = 1 << 1,
    GK_RIGHT = 1 << 2,
    GK_LEFT = 1 << 3,
    GK_FIRE = 1 << 4,
    // Backspace key, used to delete a character when the player are entering their name.
    GK_DEL = 1 << 5,
    // Return key, used by the player to confirm they have inputted their name.
    GK_CONFIRM = 1 << 6
};

enum Side { SD_PLAYER, SD_ENEMY };
enum TextDir { TEXT_DIR_LEFT, TEXT_DIR_RIGHT, TEXT_DIR_CENTER };

typedef struct {
    delegate_fn logic;
    delegate_fn render;
} Delegate;

/**
 * This structure represents a single texture used by the game.
 * The texture is cached to prevent from reloading it every time it is needed.
 */
typedef struct Texture {
    char name[TEX_NAME_LEN_MAX];
    SDL_Texture *tex;
    struct Texture *next;
} Texture;

/**
 * This structure represents an entity in the game.
 * Entities are objects like the player, the enemies, the bullets, etc.
 */
typedef struct Thing {
    int reload;
    int hp;
    enum Side side;
    SDL_FPoint delta;
    SDL_FRect rect;
    SDL_Texture *tex;
    struct Thing *next;
} Thing;

/**
 * This structure holds information about the aircrafts and their bullets
 * in the current arena they are playing in.
 */
struct Arena {
    int score;
    Thing aircraft_head;
    Thing bullet_head;
    Thing pod_head;
    Explosion expl_head;
    Debris debris_head;
    Thing *aircraft_tail;
    Thing *bullet_tail;
    Thing *pod_tail;
    Explosion *expl_tail;
    Debris *debris_tail;
};

/**
 * This structure contains the state of the game.
 * Being the main structure of the program, it is passed around between most of
 * the functions.
 */
struct State {
    bool running;
    enum GameKey key;
    Delegate dg;
    Texture tex_head;
    Texture *tex_tail;
    SDL_Renderer *ren;
    SDL_Window *win;
    char input_buf[LINE_LEN_MAX];
};

/**
 * Initialization functions.
 */
bool init_sdl(State *S);
void init_game(State *S);
void deinit_sdl(State *S);
void background_init(State *S);
bool arena_init(State *S, Arena *A);
void title_init(State *S);

/**
 * Handling functions.
 */
void handle_input(State *S);
void handle_background(int *x);
void handle_stars(Star *stars);
void handle_explosions(Arena *A);
void handle_debris(Arena *A);

/**
 * Rendering functions.
 */
void render_clear(State *S);
void render_display(State *S);
SDL_Texture *load_texture(State *S, const char *path);
void render_texture(State *S, SDL_Texture *texture, float x, float y);
void render_texture_clip(State *S, SDL_Texture *texture, SDL_Rect *src, float x, float y);
void render_aircrafts(State *S, Arena *A);
void render_bullets(State *S, Arena *A);
void render_background(State *S, int bg_x);
void render_stars(State *S, Star *stars);
void render_debris(State *S, Arena *A);
void render_explosions(State *S, Arena *A);
void render_hud(State *S, Arena *A);
void render_pods(State *S, Arena *A);

/**
 * Utility functions.
 */
void set_thing_size(Thing *T);
void bound_thing(Thing *T);
const char *get_res_path(void);
bool are_colliding(Thing *a, Thing *b);
void find_slope(float x1, float y1, float x2, float y2, float *dx, float *dy);
void strncpy_term(char *dst, const char *src, size_t len);

// These two return a random number () N where a <= N <= b.
static inline int randint(const int a, const int b) {
    return (rand() / (RAND_MAX / (b + 1 - a))) + a;
}
static inline float randfloat(const float a, const float b) {
    return (((float)rand() / (float)RAND_MAX) * (b - a)) + a;
}

/**
 * Text functions.
 */
void fonts_init(State *S);
void text_draw(State *S, int x, int y, SDL_Color color, enum TextDir dir, const char *fmt, ...);

/**
 * Miscellaneous.
 */
void add_explosions(Arena *A, float x, float y, int count);
void add_debris(Arena *A, Thing *T);
void add_pod_points(Arena *A, float x, float y);

#endif // CHOOTER_H_
