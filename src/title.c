#include "chooter.h"
#include "score.h"

static int timeout, reveal;
static SDL_Texture *sdl2_tex, *shooter_tex;

static void render_logo(State *S) {
    SDL_Rect r = {.x = 0, .y = 0};

    SDL_QueryTexture(sdl2_tex, NULL, NULL, &r.w, &r.h);

    r.h = MIN(reveal, r.h);

    render_texture_clip(S, sdl2_tex, &r, WIN_WIDTH * 0.5 - r.w * 0.5, 100);

    SDL_QueryTexture(shooter_tex, NULL, NULL, &r.w, &r.h);

    r.h = MIN(reveal, r.h);

    render_texture_clip(S, shooter_tex, &r, WIN_WIDTH * 0.5 - r.w * 0.5, 250);
}

static void logic(State *S, Arena *A) {
    handle_background(&background_x);
    handle_stars(stars);

    if (reveal < WIN_HEIGHT) {
        ++reveal;
    }

    if (--timeout < 1) {
        high_score_init(S);
    }

    if (S->key & GK_FIRE) {
        arena_init(S, A);
    }
}

static void render(State *S, Arena *A) {
    (void)A;

    render_background(S, background_x);
    render_stars(S, stars);
    render_logo(S);

    extern const SDL_Color WHITE;

    if (timeout % 40 < 20) {
        text_draw(S, WIN_WIDTH * 0.5, 600, WHITE, TEXT_DIR_CENTER, "PRESS SPACE TO START");
    }
}

void title_init(State *S) {
    S->dg.logic = logic;
    S->dg.render = render;

    S->key = 0;

    sdl2_tex = load_texture(S, "res/image/sdl2.png");
    shooter_tex = load_texture(S, "res/image/shooter.png");

    timeout = FPS * 5;
}
