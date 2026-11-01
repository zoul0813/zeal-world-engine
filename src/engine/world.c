#include "world.h"

#include <zgdk/tilemap/scroll.h>

/**
 * It is assumed the viewport is always the screen 0,1 on the tilemap
 */

#define SLIDE_SPEED 4
#define WORLD_ROOM_COUNT (WORLD_WIDTH * WORLD_HEIGHT)
#define WORLD_CHEST_STATE_SIZE (WORLD_ROOM_COUNT / 8)

Vector2_u8 world_position;
uint8_t ROOM_HAS_UNOPENED_CHEST;
static uint8_t room_tiles[ROOM_WIDTH * ROOM_HEIGHT];
static Tilemap room;
static uint8_t CHEST_OPEN[WORLD_CHEST_STATE_SIZE];
static gfx_context* world_ctx;
static room_loader_t load_room;

/** WORLD FUNCTIONS */

static uint8_t world_room_index(uint8_t x, uint8_t y) {
    return (y * WORLD_WIDTH) + x;
}

uint8_t world_chest_open(uint8_t x, uint8_t y) {
    if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return 0;
    uint8_t room = world_room_index(x, y);
    return CHEST_OPEN[room >> 3] & (1 << (room & 7));
}

static void world_set_chest_open(uint8_t x, uint8_t y) {
    uint8_t room = world_room_index(x, y);
    CHEST_OPEN[room >> 3] |= (1 << (room & 7));
}

zos_err_t world_init(gfx_context* ctx, Vector2_u8 start, room_loader_t loader) {
    if (ctx == NULL || loader == NULL || start.x >= WORLD_WIDTH || start.y >= WORLD_HEIGHT) {
        return ERR_INVALID_PARAMETER;
    }

    // load the starting room supplied by the game
    world_ctx = ctx;
    load_room = loader;
    rect_set(room.rect, ROOM_OFFSET_X, ROOM_OFFSET_Y, ROOM_WIDTH, ROOM_HEIGHT);
    zos_err_t err = tilemap_register(&room, room_tiles);
    if (err != ERR_SUCCESS) return err;
    world_position = start;
    err = load_room(ctx, world_position.x, world_position.y, 1, 0);
    if (err != ERR_SUCCESS) return err;
    tilemap_scroll_x(0, SCREEN_WIDTH);
    return ERR_SUCCESS;
}

void world_open_chest(void) {
    world_set_chest_open(world_position.x, world_position.y);
    ROOM_HAS_UNOPENED_CHEST = 0;
}

/** SCREEN FUNCTIONS */

void screen_slide(Point tilemap, const Edge edge, const Point point, const transition_callback_t callback) {
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
            err = load_room(world_ctx, world.x, world.y, 0, 0);
            if (err != ERR_SUCCESS) return err;
            break;
        case EdgeRight:
            err = load_room(world_ctx, world.x, world.y, 2, 0);
            if (err != ERR_SUCCESS) return err;
            break;
        case EdgeTop:
            // load the old map beneath
            err = load_room(world_ctx, old_world.x, old_world.y, 1, 1);
            if (err != ERR_SUCCESS) return err;
            // jump to old map
            gfx_wait_vblank(world_ctx);
            tilemap_scroll(0, SCREEN_WIDTH, SCREEN_HEIGHT);
            gfx_wait_end_vblank(world_ctx);

            // load the new map above
            err = load_room(world_ctx, world.x, world.y, 1, 0);
            if (err != ERR_SUCCESS) return err;

            tilemap.y = SCREEN_HEIGHT;
            break;
        case EdgeBottom:
            err = load_room(world_ctx, world.x, world.y, 1, 1);
            if (err != ERR_SUCCESS) return err;
            break;
    }

    screen_slide(tilemap, edge, point, callback);

    // load the current screen into 1,0 and jump to it
    err = load_room(world_ctx, world.x, world.y, 1, 0);
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
