#include "chooter.h"
#include "score.h"
#include "sound.h"

bool init_sdl(State *S) {
    S->running = true;
    S->key = 0;

    char *errorstr = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        errorstr = "could not initialize SDL: %s\n";
        goto failure;
    }

    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;

    if (!(IMG_Init(img_flags) & img_flags)) {
        errorstr = "could not initialize SDL_image: %s\n";
        goto failure;
    }

    if ((S->win = SDL_CreateWindow("chooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_RESIZABLE)) == NULL) {
        errorstr = "could not create window: %s\n";
        goto failure;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "could not set the render scale quality to linear\n");
    }

    if ((S->ren = SDL_CreateRenderer(
             S->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
        errorstr = "could not create renderer: %s\n";
        goto failure;
    }

    if (SDL_RenderSetLogicalSize(S->ren, WIN_WIDTH, WIN_HEIGHT) < 0) {
        errorstr = "could not set device independent resolution for rendering: %s\n";
        goto failure;
    }

    if (SDL_ShowCursor(SDL_DISABLE) < 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "could not hide the cursor\n");
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        fprintf(stderr, "could not initialize SDL_mixer: %s\n", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(SOUND_CHANNELS_MAX);

    return true;

failure:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, errorstr, SDL_GetError());
    return false;
}

void deinit_sdl(State *S) {
    SDL_DestroyRenderer(S->ren);
    SDL_DestroyWindow(S->win);
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void init_game(State *S) {
    background_init(S);
    stars_init(stars);
    sounds_init();
    fonts_init(S);
    high_score_table_init();
    music_load("res/music/mercury.ogg");
    music_play(true);
}
