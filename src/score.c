#include "score.h"
#include "chooter.h"

const SDL_Color WHITE = {.r = 255, .g = 255, .b = 255};
HighScore highscores[HIGH_SCORES_COUNT];

static int cursor_blink, timeout;
static HighScore *new_high_score;
static const SDL_Color YELLOW = {.r = 255, .g = 255, .b = 0};

static void handle_name_input(State *S) {
    size_t new_hs_name_len = strlen(new_high_score->player_name);

    for (size_t i = 0; S->input_buf[i]; ++i) {
        char c = toupper(S->input_buf[i]);

        if (new_hs_name_len < HIGH_SCORE_NAME_LEN_MAX - 1 && (isupper(c) || c == ' ')) {
            new_high_score->player_name[new_hs_name_len++] = c;
        }
    }

    // Is there at least 1 character and backspace is pressed?
    if (new_hs_name_len && S->key & GK_DEL) {
        // Delete the character.
        new_high_score->player_name[--new_hs_name_len] = '\0';
        // Make sure 1 backspace key press doesn't delete the entire name at once.
        S->key &= ~GK_DEL;
    }

    if (S->key & GK_CONFIRM) {
        // Did the player even enter a name, i.e., is the string empty?
        if (!*new_high_score->player_name) {
            strncpy_term(new_high_score->player_name, "NO NAME", HIGH_SCORE_NAME_LEN_MAX);
        }

        new_high_score = NULL;
    }
}

static void logic(State *S, Arena *A) {
    handle_background(&background_x);
    handle_stars(stars);

    if (new_high_score) {
        handle_name_input(S);
    } else {
        if (--timeout < 1) {
            title_init(S);
        }
        if (S->key & GK_FIRE) {
            arena_init(S, A);
        }
    }

    // Reset the counter every second.
    if (++cursor_blink >= FPS) {
        cursor_blink = 0;
    }
}

static void render_name_input(State *S) {
    text_draw(S, WIN_WIDTH * 0.5, 70, WHITE, TEXT_DIR_CENTER, "NEW HIGH SCORE!");
    text_draw(S, WIN_WIDTH * 0.5, 120, WHITE, TEXT_DIR_CENTER, "ENTER YOUR NAME:");
    text_draw(S, WIN_WIDTH * 0.5, 250, (SDL_Color){.r = 128, .g = 255, .b = 128}, TEXT_DIR_CENTER,
              new_high_score->player_name);

    if (cursor_blink < FPS * 0.5) {
        SDL_Rect cursor = {
            .x =
                ((WIN_WIDTH * 0.5) + (strlen(new_high_score->player_name) * GLYPH_WIDTH) * 0.5) + 5,
            .y = 250,
            .w = GLYPH_WIDTH,
            .h = GLYPH_HEIGHT};

        SDL_SetRenderDrawColor(S->ren, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(S->ren, &cursor);
    }

    text_draw(S, WIN_WIDTH * 0.5, 625, WHITE, TEXT_DIR_CENTER, "PRESS RETURN TO CONTINUE");
}

static void render_high_scores(State *S) {
    text_draw(S, WIN_WIDTH * 0.5, 70, WHITE, TEXT_DIR_CENTER, "HIGH SCORES");

    for (int i = 0, y = 150; i < HIGH_SCORES_COUNT; ++i, y += 50) {
        text_draw(S, WIN_WIDTH * 0.5, y, highscores[i].recent ? YELLOW : WHITE, TEXT_DIR_CENTER,
                  "#%d. %-*s ...... %03d", i + 1, HIGH_SCORE_NAME_LEN_MAX - 1,
                  highscores[i].player_name, highscores[i].score);
    }
}

static void render(State *S, Arena *A) {
    (void)A;

    render_background(S, background_x);
    render_stars(S, stars);

    if (new_high_score) {
        render_name_input(S);
    } else {
        render_high_scores(S);

        if (timeout % 40 < 20) {
            text_draw(S, WIN_WIDTH * 0.5, 600, WHITE, TEXT_DIR_CENTER, "PRESS SPACE TO START");
        }
    }
}

void high_score_table_init(void) {
    memset(highscores, 0, HIGH_SCORES_COUNT);

    for (int i = 0, s = HIGH_SCORES_COUNT * 10; i < HIGH_SCORES_COUNT; ++i, s -= 10) {
        highscores[i].score = s;
        strncpy_term(highscores[i].player_name, "NO NAME", HIGH_SCORE_NAME_LEN_MAX);
    }
}

void high_score_init(State *S) {
    S->dg.logic = logic;
    S->dg.render = render;

    // Make sure there is no switching of the high score table and the game itself instantly.
    S->key = 0;

    timeout = FPS * 5;
}

static int high_score_cmp(const void *a, const void *b) {
    return (*(HighScore *)b).score - (*(HighScore *)a).score;
}

void add_high_score(int score) {
    HighScore new[HIGH_SCORES_COUNT + 1];

    for (int i = 0; i < HIGH_SCORES_COUNT; ++i) {
        new[i] = highscores[i];
        new[i].recent = false;
    }

    new[HIGH_SCORES_COUNT].score = score;
    new[HIGH_SCORES_COUNT].recent = true;

    qsort(new, HIGH_SCORES_COUNT + 1, sizeof(HighScore), high_score_cmp);

    new_high_score = NULL;

    for (int i = 0; i < HIGH_SCORES_COUNT; ++i) {
        highscores[i] = new[i];

        if (highscores[i].recent) {
            new_high_score = &highscores[i];
        }
    }
}
