#include "chooter.h"

void background_init(State *S) {
    textures[BACKGROUND] = load_texture(S, "res/image/background.png");
    background_x = 0;
}

void stars_init(Star *stars) {
    for (int i = 0; i < STARS_MAX; ++i) {
        stars[i].pos.x = randint(0, WIN_WIDTH - 1);
        stars[i].pos.y = randint(0, WIN_HEIGHT - 1);
        stars[i].speed = randint(1, 8);
    }
}

void handle_background(int *x) {
    if (--*x < -WIN_WIDTH) {
        *x = 0;
    }
}

void handle_stars(Star *stars) {
    for (int i = 0; i < STARS_MAX; ++i) {
        stars[i].pos.x -= stars[i].speed;

        if (stars[i].pos.x < 0) {
            stars[i].pos.x = WIN_WIDTH + stars[i].pos.x;
        }
    }
}

void handle_explosions(Arena *A) {
    for (Explosion *prev = &A->expl_head, *e = A->expl_head.next; e; prev = e, e = e->next) {
        e->pos.x += e->delta.x;
        e->pos.y += e->delta.y;

        // Is the explosion completely transparent? If so, delete it.
        if (--e->color.a == 0) {
            if (e == A->expl_tail) {
                A->expl_tail = prev;
            }

            prev->next = e->next;
            free(e);
            e = prev;
        }
    }
}

void handle_debris(Arena *A) {
    for (Debris *prev = &A->debris_head, *d = A->debris_head.next; d; prev = d, d = d->next) {
        d->pos.x += d->delta.x;
        d->pos.y += d->delta.y;

        d->delta.y += 0.5;

        // Is the debris out of "life"? if so, delete it.
        if (--d->life < 1) {
            if (d == A->debris_tail) {
                A->debris_tail = prev;
            }

            prev->next = d->next;
            free(d);
            d = prev;
        }
    }
}

void add_explosions(Arena *A, float x, float y, int count) {
    while (count--) {
        Explosion *e = calloc(1, sizeof(Explosion));

        A->expl_tail->next = e;
        A->expl_tail = e;

        e->pos.x = x + randint(0, 31) - randint(0, 31);
        e->pos.y = y + randint(0, 31) - randint(0, 31);
        e->delta.x = randint(0, 9) - randint(0, 9);
        e->delta.y = randint(0, 9) - randint(0, 9);

        e->delta.x *= 0.1;
        e->delta.y *= 0.1;

        // Set the explosion to a random color.
        switch (randint(0, 3)) {
        case 0:
            // Red.
            e->color.r = 255;
            break;
        case 1:
            // Orange.
            e->color.r = 255;
            e->color.g = 128;
            break;
        case 2:
            // Yellow.
            e->color.r = 255;
            e->color.g = 255;
            break;
        case 3:
            // White.
            e->color.r = 255;
            e->color.g = 255;
            e->color.b = 255;
            break;
        }

        // How long will the explosion "live" for (between 0 to 3 seconds).
        e->color.a = randint(0, FPS - 1) * 3;
    }
}

void add_debris(Arena *A, Thing *T) {
    for (int h = T->rect.h * 0.5, y = 0; y <= h; y += h) {
        for (int w = T->rect.w * 0.5, x = 0; x <= w; x += w) {
            Debris *d = calloc(1, sizeof(Debris));

            A->debris_tail->next = d;
            A->debris_tail = d;

            d->pos.x = T->rect.x + T->rect.w * 0.5;
            d->pos.y = T->rect.y + T->rect.h * 0.5;
            d->delta.x = randint(0, 4) - randint(0, 4);
            // A random value between -5 and -16, the debris will move up screen
            // to begin with.
            d->delta.y = -randint(5, 16);
            d->life = FPS * 2;
            d->tex = T->tex;
            d->rect = (SDL_FRect){x, y, w, h};
        }
    }
}

void add_pod_points(Arena *A, float x, float y) {
    Thing *pod = calloc(1, sizeof(Thing));

    A->pod_tail->next = pod;
    A->pod_tail = pod;

    pod->rect.x = x;
    pod->rect.y = y;
    pod->delta.x = -randint(0, 4); // For the most part, the pod will move from right to left.
    // Move up/down the screen at a random speed (between -5 to 5).
    pod->delta.y = randint(0, 4) - randint(0, 4);
    pod->hp = FPS * 10; // The pod will survive for 10 seconds.
    pod->tex = textures[POD];
    set_thing_size(pod);
    // Shift the pod's position by half of its width & height to center it more accurately.
    pod->rect.x -= pod->rect.w * 0.5;
    pod->rect.y -= pod->rect.h * 0.5;
}
