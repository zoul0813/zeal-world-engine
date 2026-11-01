#include "transitions.h"
#include "internal.h"

#include <zgdk/tilemap/scroll.h>

/* The active viewport is tilemap screen (1, 0). */
#define SLIDE_SPEED 4

static void screen_slide(Point tilemap, const Edge edge, const Point point, const transition_callback_t callback) {
    uint8_t frames = 0;

    Direction world_direction = {
        .x = 0,
        .y = 0,
    };

    Direction subject_direction = {
        .x = 0,
        .y = 0,
    };

    Point subject_pos = {
        .x = point.x,
        .y = point.y,
    };

    // determine direction/timing of screen
    switch (edge) {
        case EdgeLeft:
            world_direction.x = DIRECTION_LEFT * SLIDE_SPEED;
            frames = SCREEN_WIDTH / SLIDE_SPEED;
            break;
        case EdgeRight:
            world_direction.x = DIRECTION_RIGHT * SLIDE_SPEED;
            frames = SCREEN_WIDTH / SLIDE_SPEED;
            break;
        case EdgeTop:
            world_direction.y = DIRECTION_UP * SLIDE_SPEED;
            frames = SCREEN_HEIGHT / SLIDE_SPEED;  // this will cause v-slides to be faster than h-slides :think:
            break;
        case EdgeBottom:
            world_direction.y = DIRECTION_DOWN * SLIDE_SPEED;
            frames = SCREEN_HEIGHT / SLIDE_SPEED;  // this will cause v-slides to be faster than h-slides :think:
            break;
    }

    // determine direction of the transition subject
    switch (edge) {
        case EdgeLeft:
            subject_direction.x = DIRECTION_LEFT * SLIDE_SPEED;
            break;
        case EdgeRight:
            subject_direction.x = DIRECTION_RIGHT * SLIDE_SPEED;
            break;
        case EdgeTop:
            subject_direction.y = DIRECTION_UP * SLIDE_SPEED;
            break;
        case EdgeBottom:
            subject_direction.y = DIRECTION_DOWN * SLIDE_SPEED;
            break;
    }

    // slide the transition subject the few extra pixels
    for (uint8_t x = SPRITE_WIDTH / SLIDE_SPEED; x > 0; x--) {
        subject_pos.x += subject_direction.x;
        subject_pos.y += subject_direction.y;
        callback(subject_pos.x, subject_pos.y);

        gfx_wait_vblank(world_ctx);
        sprites_render(world_ctx);
        gfx_wait_end_vblank(world_ctx);
    }

    // slide the subject/room into view
    for (uint8_t x = frames; x > 0; x--) {
        tilemap.x += world_direction.x;
        tilemap.y += world_direction.y;

        subject_pos.x += -subject_direction.x;
        subject_pos.y += -subject_direction.y;
        callback(subject_pos.x, subject_pos.y);

        gfx_wait_vblank(world_ctx);

        tilemap_scroll(0, tilemap.x, tilemap.y);
        sprites_render(world_ctx);

        gfx_wait_end_vblank(world_ctx);
    }

    // make sure the transition subject is positioned properly
    switch (edge) {
        case EdgeLeft:
            callback(SCREEN_WIDTH, point.y);
            break;
        case EdgeRight:
            callback(SPRITE_WIDTH, point.y);
            break;
        case EdgeTop:
            callback(point.x, SCREEN_HEIGHT);
            break;
        case EdgeBottom:
            callback(point.x, SPRITE_HEIGHT);
            break;
    }
}

zos_err_t screen_load(const Vector2_u8 world, const Edge edge, const Point point, transition_callback_t const callback) {
    Vector2_u8 old_world = {
        .x = world_position.x,
        .y = world_position.y,
    };

    Point tilemap = {
        .x = SCREEN_WIDTH,
        .y = 0,
    };

    zos_err_t err;
    switch (edge) {
        case EdgeLeft:
            err = world_load_room(world_ctx, world.x, world.y, 0, 0);
            if (err != ERR_SUCCESS) return err;
            break;
        case EdgeRight:
            err = world_load_room(world_ctx, world.x, world.y, 2, 0);
            if (err != ERR_SUCCESS) return err;
            break;
        case EdgeTop:
            // load the old map beneath
            err = world_load_room(world_ctx, old_world.x, old_world.y, 1, 1);
            if (err != ERR_SUCCESS) return err;
            // jump to old map
            gfx_wait_vblank(world_ctx);
            tilemap_scroll(0, SCREEN_WIDTH, SCREEN_HEIGHT);
            gfx_wait_end_vblank(world_ctx);

            // load the new map above
            err = world_load_room(world_ctx, world.x, world.y, 1, 0);
            if (err != ERR_SUCCESS) return err;

            tilemap.y = SCREEN_HEIGHT;
            break;
        case EdgeBottom:
            err = world_load_room(world_ctx, world.x, world.y, 1, 1);
            if (err != ERR_SUCCESS) return err;
            break;
    }

    screen_slide(tilemap, edge, point, callback);

    // load the current screen into 1,0 and jump to it
    err = world_load_room(world_ctx, world.x, world.y, 1, 0);
    if (err != ERR_SUCCESS) return err;
    gfx_wait_vblank(world_ctx);
    tilemap_scroll(0, SCREEN_WIDTH, 0);
    gfx_wait_end_vblank(world_ctx);

    // update to the latest
    world_position.x = world.x;
    world_position.y = world.y;
    return ERR_SUCCESS;
}

/**
 * Transition to the next screen
 *
 * This will momentarily "pause" the game and transition
 * the mext screen into view, if applicable.
 *
 * @param edge The edge of the current screen the subject is exiting from
 * @returns 1 after a transition, 0 at a boundary, or a negated load error
 */
int16_t screen_transition(const Edge edge, const Point point, const transition_callback_t callback) {
    uint8_t transition = 0;

    Vector2_u8 new_world = {
        .x = world_position.x,
        .y = world_position.y,
    };

    switch (edge) {
        case EdgeLeft:
            if (new_world.x > 0) {
                new_world.x--;
                transition = 1;
            } else {
                // keep the transition subject on screen
                callback(SPRITE_WIDTH, point.y);
            }
            break;
        case EdgeRight:
            if (new_world.x < (WORLD_WIDTH - 1)) {
                new_world.x++;
                transition = 1;
            } else {
                // keep the transition subject on screen
                callback(SCREEN_WIDTH, point.y);
            }
            break;
        case EdgeTop:
            if (new_world.y > 0) {
                new_world.y--;
                transition = 1;
            } else {
                // keep the transition subject on screen
                callback(point.x, SPRITE_HEIGHT);
            }
            break;
        case EdgeBottom:
            if (new_world.y < (WORLD_HEIGHT - 1)) {
                new_world.y++;
                transition = 1;
            } else {
                // keep the transition subject on screen
                callback(point.x, SCREEN_HEIGHT);
            }
            break;
    }

    if (transition) {
        zos_err_t err = screen_load(new_world, edge, point, callback);
        if (err != ERR_SUCCESS) return -(int16_t)err;
        return 1;
    }

    return 0;
}
