#include <stdlib.h>
#include <string.h>

#include "chooter.h"
#include "score.h"
#include "sound.h"

int background_x;
Star stars[STARS_MAX];
SDL_Texture *textures[LEN];

static int enemy_spawn_rate, arena_reset_timer;
static Thing *player;

static void fire_bullet(Arena *A) {
    Thing *bullet = calloc(1, sizeof(Thing));

    A->bullet_tail->next = bullet;
    A->bullet_tail = bullet;

    bullet->rect.x = player->rect.x;
    bullet->rect.y = player->rect.y;
    bullet->delta.x = PLAYER_BULLET_SPD;
    bullet->hp = 1;
    bullet->tex = textures[P_BULLET];
    bullet->side = player->side;
    set_thing_size(bullet);
    bullet->rect.y += player->rect.h * 0.5 - bullet->rect.h * 0.5;
    bullet->side = SD_PLAYER;
    player->reload = 8; // The player can fire once every 8 frames.
}

static void fire_enemy_bullet(Arena *A, Thing *T) {
    Thing *bullet = calloc(1, sizeof(Thing));

    A->bullet_tail->next = bullet;
    A->bullet_tail = bullet;

    bullet->rect.x = T->rect.x;
    bullet->rect.y = T->rect.y;
    bullet->hp = 1;
    bullet->tex = textures[E_BULLET];
    bullet->side = T->side;
    set_thing_size(bullet);

    bullet->rect.x += T->rect.w * 0.5 - bullet->rect.w * 0.5;
    bullet->rect.y += T->rect.h * 0.5 - bullet->rect.h * 0.5;

    find_slope(player->rect.x + player->rect.w * 0.5, player->rect.y + player->rect.h * 0.5,
               T->rect.x, T->rect.y, &bullet->delta.x, &bullet->delta.y);

    bullet->delta.x *= ENEMY_BULLET_SPD;
    bullet->delta.y *= ENEMY_BULLET_SPD;

    // Make sure the bullet will only hit the player,
    // skipping the issuing enemy as well as any others it may happen to collide
    // with.
    bullet->side = SD_ENEMY;

    // The enemy may fire again anytime within the next two seconds.
    T->reload = randint(0, FPS - 1) * 2;
}

static bool aircraft_got_shot(Arena *A, Thing *bullet) {
    for (Thing *ac = A->aircraft_head.next; ac; ac = ac->next) {
        if (ac->side != bullet->side && are_colliding(bullet, ac)) {
            bullet->hp = 0;
            ac->hp = 0;

            add_explosions(A, ac->rect.x, ac->rect.y, 32);
            add_debris(A, ac);

            if (ac == player) {
                sound_play(SND_PLAYER_DEATH, CHNL_PLAYER);
            } else {
                sound_play(SND_ENEMY_DEATH, CHNL_ANY);
                add_pod_points(A, ac->rect.x + ac->rect.w * 0.5, ac->rect.y + ac->rect.h * 0.5);
            }

            return true;
        }
    }

    return false;
}

static void perform_player(State *S, Arena *A) {
    player->delta.x = 0;
    player->delta.y = 0;

    if (player->reload > 0) {
        --player->reload;
    }

    if (S->key & GK_UP) {
        player->delta.y = -PLAYER_SPD;
    }
    if (S->key & GK_DOWN) {
        player->delta.y = PLAYER_SPD;
    }
    if (S->key & GK_RIGHT) {
        player->delta.x = PLAYER_SPD;
    }
    if (S->key & GK_LEFT) {
        player->delta.x = -PLAYER_SPD;
    }
    if (S->key & GK_FIRE && player->reload < 1) {
        sound_play(SND_PLAYER_BULLET, CHNL_PLAYER);
        fire_bullet(A);
    }
}

static void bound_player(void) {
    if (player->rect.x < 0) {
        player->rect.x = 0;
    } else if (player->rect.x > WIN_WIDTH - player->rect.w) {
        player->rect.x = WIN_WIDTH - player->rect.w;
    }

    if (player->rect.y < 0) {
        player->rect.y = 0;
    } else if (player->rect.y > WIN_HEIGHT - player->rect.h) {
        player->rect.y = WIN_HEIGHT - player->rect.h;
    }
}

static void perform_bullets(Arena *A) {
    for (Thing *prev = &A->bullet_head, *b = A->bullet_head.next; b; prev = b, b = b->next) {
        b->rect.x += b->delta.x;
        b->rect.y += b->delta.y;

        if (aircraft_got_shot(A, b) || b->rect.x < -b->rect.w || b->rect.x > WIN_WIDTH ||
            b->rect.y < -b->rect.h || b->rect.y > WIN_HEIGHT) {
            // Is it the last bullet in the list?
            if (b == A->bullet_tail) {
                A->bullet_tail = prev;
            }

            // Remove the bullet from the list.
            prev->next = b->next;
            free(b);
            b = prev;
        }
    }
}

static void perform_aircrafts(Arena *A) {
    // ac = aircraft, I'm lazy - shoot me
    for (Thing *prev = &A->aircraft_head, *ac = A->aircraft_head.next; ac;
         prev = ac, ac = ac->next) {
        ac->rect.x += ac->delta.x;
        ac->rect.y += ac->delta.y;

        if (ac != player && ac->rect.x < -ac->rect.w) {
            ac->hp = 0;
        }

        if (ac->hp == 0) {
            if (ac == player) {
                player = NULL;
            }

            if (ac == A->aircraft_tail) {
                A->aircraft_tail = prev;
            }

            prev->next = ac->next;
            free(ac);
            ac = prev;
        }
    }
}

static void perform_enemies(Arena *A) {
    for (Thing *enemy = A->aircraft_head.next; enemy; enemy = enemy->next) {
        if (enemy != player) {
            enemy->rect.y = MIN(MAX(enemy->rect.y, 0), WIN_HEIGHT - enemy->rect.h);

            if (player && --enemy->reload < 1) {
                fire_enemy_bullet(A, enemy);
                sound_play(SND_ENEMY_BULLET, CHNL_ENEMY_BULLET);
            }
        }
    }
}

static void perform_pods(Arena *A) {
    for (Thing *prev = &A->pod_head, *pod = A->pod_head.next; pod; prev = pod, pod = pod->next) {
        if (pod->rect.x < 0) {
            pod->rect.x = 0;
            pod->delta.x = -pod->delta.x;
        }

        if (pod->rect.x + pod->rect.w > WIN_WIDTH) {
            pod->rect.x = WIN_WIDTH - pod->rect.w;
            pod->delta.x = -pod->delta.x;
        }

        if (pod->rect.y < 0) {
            pod->rect.y = 0;
            pod->delta.y = -pod->delta.y;
        }

        if (pod->rect.y + pod->rect.h > WIN_HEIGHT) {
            pod->rect.y = WIN_HEIGHT - pod->rect.h;
            pod->delta.y = -pod->delta.y;
        }

        pod->rect.x += pod->delta.x;
        pod->rect.y += pod->delta.y;

        if (player && are_colliding(pod, player)) {
            pod->hp = 0;
            A->score += 10;
            // highscore = MAX(A->score, highscore);
            sound_play(SND_POD, CHNL_POD);
        }

        if (--pod->hp < 1) {
            if (pod == A->pod_tail) {
                A->pod_tail = prev;
            }

            prev->next = pod->next;
            free(pod);
            pod = prev;
        }
    }
}

static void spawn_enemies(Arena *A) {
    Thing *enemy = calloc(1, sizeof(Thing));

    A->aircraft_tail->next = enemy;
    A->aircraft_tail = enemy;

    enemy->rect.x = WIN_WIDTH;
    enemy->rect.y = randint(0, WIN_HEIGHT - 1);
    enemy->tex = textures[ENEMY];
    set_thing_size(enemy);
    enemy->delta.x = -randint(2, 5);
    enemy->delta.y = randfloat(-1.f, 1.f);
    enemy->side = SD_ENEMY;
    enemy->hp = 1;
    // Set the enemy's reload to 1 - 3 seconds, in order to give the player a
    // chance to destroy it.
    enemy->reload = (1 + randint(0, 2)) * FPS;
    // A new enemy is spawned every 500-1500ms.
    enemy_spawn_rate = 30 + randint(0, FPS - 1);
}

static void logic(State *S, Arena *A) {
    handle_background(&background_x);
    handle_stars(stars);

    if (player) {
        perform_player(S, A);
    }

    perform_enemies(A);
    perform_aircrafts(A);
    perform_bullets(A);

    handle_explosions(A);
    handle_debris(A);

    perform_pods(A);

    if (--enemy_spawn_rate < 1) {
        spawn_enemies(A);
    }

    if (player) {
        bound_player();
    }

    if (player == NULL && --arena_reset_timer < 1) {
        add_high_score(A->score);
        high_score_init(S);
    }
}

static void render(State *S, Arena *A) {
    render_background(S, background_x);
    render_stars(S, stars);
    render_pods(S, A);
    render_aircrafts(S, A);
    render_debris(S, A);
    render_explosions(S, A);
    render_bullets(S, A);
    render_hud(S, A);
}

static bool init_player(Arena *A) {
    if ((player = calloc(1, sizeof(Thing))) == NULL) {
        return false;
    }

    A->aircraft_tail->next = player;
    A->aircraft_tail = player;

    player->rect.x = 100;
    player->rect.y = 100;
    player->hp = 1;
    player->tex = textures[PLAYER];
    set_thing_size(player);
    player->side = SD_PLAYER;

    return true;
}

static void free_list(Thing *list) {
    while (list->next) {
        Thing *tmp = list->next;
        list->next = tmp->next;
        free(tmp);
    }
}

static void reset_arena(Arena *A) {
    free_list(&A->aircraft_head);
    free_list(&A->bullet_head);

    // TODO: make these 2 work using pseudo-polymorphism.
    // free_list(&A->expl_head);
    // free_list(&A->debris_head);

    while (A->expl_head.next) {
        Explosion *tmp = A->expl_head.next;
        A->expl_head.next = tmp->next;
        free(tmp);
    }

    while (A->debris_head.next) {
        Debris *tmp = A->debris_head.next;
        A->debris_head.next = tmp->next;
        free(tmp);
    }

    free_list(&A->pod_head);

    A->aircraft_tail = &A->aircraft_head;
    A->bullet_tail = &A->bullet_head;
    A->expl_tail = &A->expl_head;
    A->debris_tail = &A->debris_head;
    A->pod_tail = &A->pod_head;
}

bool arena_init(State *S, Arena *A) {
    S->dg.logic = logic;
    S->dg.render = render;

    memset(A, 0, sizeof(Arena));

    A->aircraft_tail = &A->aircraft_head;
    A->bullet_tail = &A->bullet_head;
    A->expl_tail = &A->expl_head;
    A->debris_tail = &A->debris_head;
    A->pod_tail = &A->pod_head;

    // Precache the textures, to avoid loading them each time.
    textures[PLAYER] = load_texture(S, "res/image/player.png");
    textures[ENEMY] = load_texture(S, "res/image/enemy.png");
    textures[P_BULLET] = load_texture(S, "res/image/player_bullet.png");
    textures[E_BULLET] = load_texture(S, "res/image/enemy_bullet.png");
    textures[EXPLOSION] = load_texture(S, "res/image/explosion.png");
    textures[POD] = load_texture(S, "res/image/pod.png");

    // We don't want the player to shoot when the spacebar is pressed to start the game, do we?
    S->key = 0;

    reset_arena(A);

    if (!init_player(A)) {
        return false;
    }

    A->score = 0;
    enemy_spawn_rate = 0; // Actually unnecessary (global variables are initialized to 0 by
                          // default), but I like it, it's consistent.
    arena_reset_timer = FPS * 3;

    return true;
}
