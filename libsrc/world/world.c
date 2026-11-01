#include "world.h"
#include "internal.h"

#include <zgdk/tilemap/scroll.h>

Vector2_u8 world_position;
gfx_context* world_ctx;
room_loader_t world_load_room;
static uint8_t room_tiles[ROOM_WIDTH * ROOM_HEIGHT];
static Tilemap room;

zos_err_t world_init(gfx_context* ctx, Vector2_u8 start, room_loader_t loader) {
    if (ctx == NULL || loader == NULL || start.x >= WORLD_WIDTH || start.y >= WORLD_HEIGHT) {
        return ERR_INVALID_PARAMETER;
    }

    // load the starting room supplied by the game
    world_ctx = ctx;
    world_load_room = loader;
    rect_set(room.rect, ROOM_OFFSET_X, ROOM_OFFSET_Y, ROOM_WIDTH, ROOM_HEIGHT);
    zos_err_t err = tilemap_register(&room, room_tiles);
    if (err != ERR_SUCCESS) return err;
    world_position = start;
    err = world_load_room(ctx, world_position.x, world_position.y, 1, 0);
    if (err != ERR_SUCCESS) return err;
    tilemap_scroll_x(0, SCREEN_WIDTH);
    return ERR_SUCCESS;
}

