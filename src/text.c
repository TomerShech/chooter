#include "chooter.h"

static SDL_Texture *font_tex;
static char text_buf[LINE_LEN_MAX] = {0};

void fonts_init(State *S) { font_tex = load_texture(S, "res/image/font.png"); }

void text_draw(State *S, int x, int y, SDL_Color color, enum TextDir dir, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(text_buf, LINE_LEN_MAX, fmt, ap);
    va_end(ap);

    SDL_Rect rect = {.y = 0, .w = GLYPH_WIDTH, .h = GLYPH_HEIGHT};

    SDL_SetTextureColorMod(font_tex, color.r, color.g, color.b);

    const size_t len = strlen(text_buf);

    if (dir == TEXT_DIR_RIGHT) {
        x -= len * GLYPH_WIDTH;
    } else if (dir == TEXT_DIR_CENTER) {
        x -= len * GLYPH_WIDTH * 0.5;
    }

    char c;

    for (size_t i = 0; (c = text_buf[i]); ++i) {
        if (c >= ' ' && c <= 'Z') {
            rect.x = (c - ' ') * GLYPH_WIDTH;

            render_texture_clip(S, font_tex, &rect, x, y);

            x += GLYPH_WIDTH;
        }
    }
}
