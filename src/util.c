#include <math.h>
#include <string.h>

#include "chooter.h"

void set_thing_size(Thing *T) {
    int w = 0, h = 0;

    SDL_QueryTexture(T->tex, NULL, NULL, &w, &h);

    T->rect.w = w;
    T->rect.h = h;
}

void bound_thing(Thing *T) {
    if (T->rect.x < 0) {
        T->rect.x = 0;
    } else if (T->rect.x > WIN_WIDTH - T->rect.w) {
        T->rect.x = WIN_WIDTH - T->rect.w;
    }

    if (T->rect.y < 0) {
        T->rect.y = 0;
    } else if (T->rect.y > WIN_HEIGHT - T->rect.h) {
        T->rect.y = WIN_HEIGHT - T->rect.h;
    }
}

const char *get_res_path(void) {
    // This will hold the base resource path: chooter/res/.
    // It has static lifetime so only one call to SDL_GetBasePath() is needed.
    static char *base_res;

    if (base_res == NULL) {
        char *base_path = SDL_GetBasePath();

        if (base_path == NULL) {
            fprintf(stderr, "could not get resource path: %s\n", SDL_GetError());
            return "";
        }

        base_res = base_path;
        SDL_free(base_path);

        // Replace the last "bin" with "res" to get the the resource path.
        char *s = strstr(base_res, "bin");
        const size_t pos = s - base_res;

        base_res[pos] = 'r';
        base_res[pos + 1] = 'e';
        base_res[pos + 2] = 's';
    }

    return base_res;
}

bool are_colliding(Thing *a, Thing *b) {
    SDL_Rect a_r = {a->rect.x, a->rect.y, a->rect.w, a->rect.h},
             b_r = {b->rect.x, b->rect.y, b->rect.w, b->rect.h};

    return SDL_HasIntersection(&a_r, &b_r);
}

void find_slope(float x1, float y1, float x2, float y2, float *dx, float *dy) {
    float steps = MAX(fabsf(x1 - x2), fabsf(y1 - y2));

    if (steps == 0) {
        *dx = 0;
        *dy = 0;
    } else {
        *dx = (float)(x1 - x2) / steps;
        *dy = (float)(y1 - y2) / steps;
    }
}

// Copy as many characters from `src` to `dst` as will fit and null terminate the result.
// This is because strncpy() doesn't guarantee to null terminate the string itself, so this must be
// done explicitly.
void strncpy_term(char *dst, const char *src, size_t len) {
    strncpy(dst, src, len);
    dst[len - 1] = '\0';
}
