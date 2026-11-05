#include "game_world.h"

#include <zgdk.h>

#include "main.h"
#include "assets.h"
#include "chests.h"
#include "player.h"
#include "zgdk/rooms.h"
#include "zgdk/rooms/transitions.h"
#include "zgdk/rooms/slide.h"
#include "zgdk/minimap.h"

static uint8_t room_tiles[ROOM_WIDTH * ROOM_HEIGHT];
static uint8_t minimap_visited[MINIMAP_STATE_BYTES(WORLD_WIDTH, WORLD_HEIGHT)];
static uint8_t minimap_special[MINIMAP_STATE_BYTES(WORLD_WIDTH, WORLD_HEIGHT)];
static Point slide_start;

static void room_entered(gfx_context* ctx, uint8_t x, uint8_t y, Edge edge);
static void draw_slide_frame(gfx_context* ctx, Edge edge, uint16_t progress);

static const rooms_slide_config_t slide_config = { .draw_frame = draw_slide_frame };
static const rooms_config_t rooms_config = {
    .size = { WORLD_WIDTH, WORLD_HEIGHT },
    .screen = { SCREEN_WIDTH, SCREEN_HEIGHT },
    .room = { .x = ROOM_OFFSET_X, .y = ROOM_OFFSET_Y, .w = ROOM_WIDTH, .h = ROOM_HEIGHT },
    .layer = LEVEL_LAYER,
    .tiles = room_tiles,
    .tiles_size = sizeof(room_tiles),
    .load_room = load_tilemap,
    .on_enter = room_entered,
    .transition = &ROOMS_TRANSITION_SLIDE,
    .transition_data = &slide_config,
};
static const minimap_state_config_t minimap_state = {
    .size = { WORLD_WIDTH, WORLD_HEIGHT },
    .visited = minimap_visited,
    .special = minimap_special,
    .capacity = sizeof(minimap_visited),
};
static const minimap_render_config_t minimap_rendering = {
    .position = { WIDTH - 2, 0 },
    .layer = UI_LAYER,
    .first_tile = 0xFD,
    .colors = { .unvisited = 23, .visited = 27, .special = 36, .current = 16 },
};

/* The demo owns positioning and all sprite animation, including the short
 * walk off the screen before the world starts scrolling. */
static void room_entered(gfx_context* ctx, uint8_t x, uint8_t y, Edge edge) {
    (void)ctx; (void)x; (void)y;
    switch (edge) {
        case EdgeLeft: player_set_xy(SCREEN_WIDTH, player.point.y); break;
        case EdgeRight: player_set_xy(SPRITE_WIDTH, player.point.y); break;
        case EdgeTop: player_set_xy(player.point.x, SCREEN_HEIGHT); break;
        case EdgeBottom: player_set_xy(player.point.x, SPRITE_HEIGHT); break;
        default: break;
    }
}

static void draw_slide_frame(gfx_context* ctx, Edge edge, uint16_t progress) {
    int8_t dx = edge == EdgeRight ? 1 : edge == EdgeLeft ? -1 : 0;
    int8_t dy = edge == EdgeBottom ? 1 : edge == EdgeTop ? -1 : 0;
    player_set_xy((int16_t)slide_start.x - dx * (int16_t)progress,
                  (int16_t)slide_start.y - dy * (int16_t)progress);
    sprites_render(ctx);
}

static void prepare_player_slide(gfx_context* ctx, Edge edge) {
    int8_t dx = edge == EdgeRight ? 4 : edge == EdgeLeft ? -4 : 0;
    int8_t dy = edge == EdgeBottom ? 4 : edge == EdgeTop ? -4 : 0;
    for (uint8_t frame = 0; frame < SPRITE_WIDTH / 4; frame++) {
        player_set_xy(player.point.x + dx, player.point.y + dy);
        gfx_wait_vblank(ctx);
        sprites_render(ctx);
        gfx_wait_end_vblank(ctx);
    }
    slide_start = player.point;
}

static void draw_rooms_position(gfx_context* ctx) {
    format_u16_3_pair(buffer, rooms_position.x, rooms_position.y);
    nprint_string(ctx, buffer, 7, 0, 0);
}

zos_err_t game_world_init(gfx_context* ctx) {
    Vector2_u8 start = { .x = WORLD_START_X, .y = WORLD_START_Y };
    zos_err_t err = rooms_init(ctx, &rooms_config, start);
    if (err != ERR_SUCCESS) return err;
    err = minimap_init(ctx, &minimap_state, &minimap_rendering, rooms_position);
    if (err != ERR_SUCCESS) return err;
    draw_rooms_position(ctx);
    return ERR_SUCCESS;
}

zos_err_t game_world_transition(gfx_context* ctx, Edge edge) {
    // The game marks rooms with unopened chests as special on the minimap.
    uint8_t previous_is_special = ROOM_HAS_UNOPENED_CHEST;
    Vector2_u8 neighbor;
    if (rooms_neighbor(edge, &neighbor)) prepare_player_slide(ctx, edge);
    int16_t transitioned = rooms_move(edge);
    if (transitioned < 0) return (zos_err_t)-transitioned;
    if (transitioned) {
        zos_err_t err = minimap_set_position(ctx, rooms_position, previous_is_special);
        if (err != ERR_SUCCESS) return err;
        draw_rooms_position(ctx);
    }
    return ERR_SUCCESS;
}
