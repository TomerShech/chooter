#include "chooter.h"
#include "score.h"

void render_clear(State *S) {
    SDL_SetRenderDrawColor(S->ren, 32, 32, 32, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(S->ren);
}

void render_display(State *S) { SDL_RenderPresent(S->ren); }

static SDL_Texture *get_texture(State *S, const char *name) {
    for (Texture *t = S->tex_head.next; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            return t->tex;
        }
    }

    return NULL;
}

static void cache_texture(State *S, const char *name, SDL_Texture *sdl_tex) {
    Texture *t = calloc(1, sizeof(Texture));

    if (t == NULL) {
        fputs("could not allocate a new texture to cache...\n", stderr);
        return;
    }

    S->tex_tail->next = t;
    S->tex_tail = t;

    strncpy_term(t->name, name, TEX_NAME_LEN_MAX);
    t->tex = sdl_tex;
}

SDL_Texture *load_texture(State *S, const char *path) {
    SDL_assert(path);

    SDL_Texture *tex = get_texture(S, path);

    // The texture wasn't previously loaded, load it now.
    if (tex == NULL) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "loading image \"%s\"", path);
        tex = IMG_LoadTexture(S->ren, path);
        cache_texture(S, path, tex);
    }

    // The texture is already cached, just return it.
    return tex;
}

void render_texture(State *S, SDL_Texture *texture, float x, float y) {
    SDL_FRect dst;

    dst.x = x;
    dst.y = y;

    int w, h;

    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    dst.w = w;
    dst.h = h;

    SDL_RenderCopyF(S->ren, texture, NULL, &dst);
}

void render_texture_clip(State *S, SDL_Texture *texture, SDL_Rect *src, float x, float y) {
    SDL_RenderCopyF(S->ren, texture, src, &(SDL_FRect){x, y, src->w, src->h});
}

void render_aircrafts(State *S, Arena *A) {
    for (Thing *aircraft = A->aircraft_head.next; aircraft; aircraft = aircraft->next) {
        render_texture(S, aircraft->tex, aircraft->rect.x, aircraft->rect.y);
    }
}

void render_bullets(State *S, Arena *A) {
    for (Thing *b = A->bullet_head.next; b; b = b->next) {
        render_texture(S, b->tex, b->rect.x, b->rect.y);
    }
}

void render_background(State *S, int bg_x) {
    for (; bg_x < WIN_WIDTH; bg_x += WIN_WIDTH) {
        SDL_RenderCopy(S->ren, textures[BACKGROUND], NULL,
                       &(SDL_Rect){bg_x, 0, WIN_WIDTH, WIN_HEIGHT});
    }
}

void render_stars(State *S, Star *stars) {
    for (int i = 0; i < STARS_MAX; ++i) {
        // Set the color of the line according to the speed of the star.
        // The higher the speed is, the brighter the star's color.
        int color = stars[i].speed * 32;

        SDL_SetRenderDrawColor(S->ren, color, color, color, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(S->ren, stars[i].pos.x, stars[i].pos.y, stars[i].pos.x + 3,
                           stars[i].pos.y);
    }
}

void render_debris(State *S, Arena *A) {
    for (Debris *d = A->debris_head.next; d; d = d->next) {
        SDL_Rect tmp = {d->rect.x, d->rect.y, d->rect.w, d->rect.h};
        render_texture_clip(S, d->tex, &tmp, d->pos.x, d->pos.y);
    }
}

void render_explosions(State *S, Arena *A) {
    SDL_SetRenderDrawBlendMode(S->ren, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(textures[EXPLOSION], SDL_BLENDMODE_ADD);

    for (Explosion *e = A->expl_head.next; e; e = e->next) {
        SDL_SetTextureColorMod(textures[EXPLOSION], e->color.r, e->color.g, e->color.b);
        SDL_SetTextureAlphaMod(textures[EXPLOSION], e->color.a);
        render_texture(S, textures[EXPLOSION], e->pos.x, e->pos.y);
    }

    SDL_SetRenderDrawBlendMode(S->ren, SDL_BLENDMODE_NONE);
}

void render_hud(State *S, Arena *A) {
    extern const SDL_Color WHITE;

    text_draw(S, 10, 10, WHITE, TEXT_DIR_LEFT, "SCORE %03d", A->score);

    if (A->score >= highscores[0].score) {
        text_draw(S, WIN_WIDTH - 10, 10, (SDL_Color){.g = 255}, TEXT_DIR_RIGHT, "HIGH SCORE %03d",
                  A->score);
    } else {
        text_draw(S, WIN_WIDTH - 10, 10, WHITE, TEXT_DIR_RIGHT, "HIGH SCORE %03d",
                  highscores[0].score);
    }
}

void render_pods(State *S, Arena *A) {
    for (Thing *pod = A->pod_head.next; pod; pod = pod->next) {
        // Make pods that have less than 2 seconds to live flash.
        if (pod->hp > FPS * 2 || pod->hp % 12 < 6) {
            render_texture(S, textures[POD], pod->rect.x, pod->rect.y);
        }
    }
}
