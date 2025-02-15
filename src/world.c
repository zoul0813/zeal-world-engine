#include <zgdk.h>

#include "assets.h"
#include "main.h"
#include "player.h"

/**
 * It is assumed the viewport is always the screen 0,1 on the tilemap
 */

#define SLIDE_SPEED 4

Vector2_u8 world_position;
TileType screen[SCREEN_HEIGHT][SCREEN_WIDTH];

/** WORLD FUNCTIONS */

error world_init(gfx_context *ctx) {
    // load the screen the player starts on
    world_position.x = 1;
    world_position.y = 1;
    load_tilemap(ctx, world_position.x, world_position.y, 1, 0);
    tilemap_scroll_x(0, SCREEN_WIDTH);
    return ERR_SUCCESS;
}

void world_set_tile(uint8_t x, uint8_t y, uint8_t tile) {
    // place tile on screen
    gfx_tilemap_place(&vctx, tile, 0, x + WIDTH, y);
    screen[y][x] = TILE_OPEN;

    // place tile into world
    tilemap[world_position.y][world_position.x][(y * WIDTH) + x] = tile;
}



/** SCREEN FUNCTIONS */

void screen_slide(Point tilemap, const Edge world_edge, const Edge player_edge) {
    uint8_t frames = 0;

    Direction world_direction = {
        .x = 0,
        .y = 0,
    };

    Direction player_direction = {
        .x = 0,
        .y = 0,
    };

    Point player_pos = {
        .x = player.point.x,
        .y = player.point.y,
    };

    // determine direction/timing of screen
    switch(world_edge) {
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
            frames = SCREEN_HEIGHT / SLIDE_SPEED; // this will cause v-slides to be faster than h-slides :think:
            break;
        case EdgeBottom:
            world_direction.y = DIRECTION_DOWN * SLIDE_SPEED;
            frames = SCREEN_HEIGHT / SLIDE_SPEED; // this will cause v-slides to be faster than h-slides :think:
            break;
    }

    // determine direction of player
    switch(player_edge) {
        case EdgeLeft:
            player_direction.x = DIRECTION_LEFT * SLIDE_SPEED;
            break;
        case EdgeRight:
            player_direction.x = DIRECTION_RIGHT * SLIDE_SPEED;
            break;
        case EdgeTop:
            player_direction.y = DIRECTION_UP * SLIDE_SPEED;
            break;
        case EdgeBottom:
            player_direction.y = DIRECTION_DOWN * SLIDE_SPEED;
            break;
    }

    // slide the player the few extra pixels
    for(uint8_t x = SPRITE_WIDTH/SLIDE_SPEED; x > 0; x--) {
        player_pos.x += player_direction.x;
        player_pos.y += player_direction.y;
        player_set_xy(player_pos.x, player_pos.y);

        gfx_wait_vblank(&vctx);
        sprites_render(&vctx);
        gfx_wait_end_vblank(&vctx);
    }

    // slide the player/room into view
    for(uint8_t x = frames; x > 0; x--) {
        tilemap.x += world_direction.x;
        tilemap.y += world_direction.y;

        player_pos.x += -player_direction.x;
        player_pos.y += -player_direction.y;
        player_set_xy(player_pos.x, player_pos.y);

        gfx_wait_vblank(&vctx);

        tilemap_scroll(0, tilemap.x, tilemap.y);
        sprites_render(&vctx);

        gfx_wait_end_vblank(&vctx);
    }

    // make sure the player is positioned properly
    switch (player_edge) {
        case EdgeLeft:
            player_set_xy(SCREEN_WIDTH, player.point.y);
            break;
        case EdgeRight:
            player_set_xy(SPRITE_WIDTH, player.point.y);
            break;
        case EdgeTop:
            player_set_xy(player.point.x, SCREEN_HEIGHT);
            break;
        case EdgeBottom:
            player_set_xy(player.point.x, SPRITE_HEIGHT);
            break;
    }
}


void screen_load(Vector2_u8 world, Edge edge) {
    Vector2_u8 old_world = {
        .x = world_position.x,
        .y = world_position.y,
    };

    Edge world_edge = edge;
    Edge player_edge = edge;

    Point tilemap = {
        .x = SCREEN_WIDTH,
        .y = 0,
    };

    switch(edge) {
        case EdgeLeft:
            load_tilemap(&vctx, world.x, world.y, 0, 0);
            break;
        case EdgeRight:
            load_tilemap(&vctx, world.x, world.y, 2, 0);
            break;
        case EdgeTop:
            // load the old map beneath
            load_tilemap(&vctx, old_world.x, old_world.y, 1, 1);
            // jump to old map
            gfx_wait_vblank(&vctx);
            tilemap_scroll(0, SCREEN_WIDTH, SCREEN_HEIGHT);
            gfx_wait_end_vblank(&vctx);

            // load the new map above
            load_tilemap(&vctx, world.x, world.y, 1, 0);

            tilemap.y = SCREEN_HEIGHT;
            break;
        case EdgeBottom:
            load_tilemap(&vctx, world.x, world.y, 1, 1);
            break;
    }

    screen_slide(tilemap, world_edge, player_edge);

    // load the current screen into 1,0 and jump to it
    load_tilemap(&vctx, world.x, world.y, 1, 0);
    gfx_wait_vblank(&vctx);
    tilemap_scroll(0, SCREEN_WIDTH, 0);
    gfx_wait_end_vblank(&vctx);

    // update to the latest
    world_position.x = world.x;
    world_position.y = world.y;
}

TileType screen_get_tile(uint8_t x, uint8_t y) {
    return screen[y][x];
}

/**
 * Transition to the next screen
 *
 * This will momentarily "pause" the game and transition
 * the mext screen into view, if applicable.
 *
 * @param edge The edge of the current screen the player is exiting from
 * @returns 1 if transition occurred, 0 is no transition occurred
 */
uint8_t screen_transition(const Edge edge) {
    uint8_t transition = 0;

    Vector2_u8 new_world = {
        .x = world_position.x,
        .y = world_position.y,
    };

    switch (edge) {
        case EdgeLeft:
            if (new_world.x > 0) {
                new_world.x--;
                transition=1;
            } else {
                // don't let the player go off screen
                player_set_xy(SPRITE_WIDTH, player.point.y);
            }
            break;
        case EdgeRight:
            if (new_world.x < (WORLD_WIDTH - 1)) {
                new_world.x++;
                transition=1;
            } else {
                // don't let the player go off screen
                player_set_xy(SCREEN_WIDTH, player.point.y);
            }
            break;
        case EdgeTop:
            if (new_world.y > 0) {
                new_world.y--;
                transition=1;
            } else {
                // don't let the player go off screen
                player_set_xy(player.point.x, SPRITE_HEIGHT);
            }
            break;
        case EdgeBottom:
            if (new_world.y < (WORLD_HEIGHT - 1)) {
                new_world.y++;
                transition=1;
            } else {
                // don't let the player go off screen
                player_set_xy(player.point.x, SCREEN_HEIGHT);
            }
            break;
    }

    if(transition) {
        screen_load(new_world, edge);
        return 1;
    }

    return 0;
}



/** TILEMAP FUNCTIONS */

void load_tilemap(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y)
{
    uint8_t line[20];
    uint8_t* map = &tilemap[y][x][0];

    for (uint16_t row = 0; row < 15; row++) {
        uint16_t offset = row * 20;
        for (uint16_t col = 0; col < 20; col++) {
            uint8_t tile = map[offset + col];
            TileType type = TILE_OPEN;

            if((tile >= 1) && (tile <= 13)) {
                type = TILE_WALL;
            } else if((tile >= 16) && (tile <= 28)) {
                type = TILE_WATER;
            } else if(tile == 43) {
                type = TILE_CHEST;
            } else if(tile > 0) {
                type = TILE_UNKNOWN;
            }

            screen[row][col] = type;

            line[col] = tile;
        }
        gfx_tilemap_load(ctx, line, 20, 0, (map_x * 20), (map_y * 15) + row);
    }
}



/** ASSETS */

void __world_assets__(void) __naked {
    __asm__(
        "_tilemap:\n"
        "    .incbin \"assets/maps/world-0000.ztm\"\n" // 0,0
        "    .incbin \"assets/maps/world-0001.ztm\"\n" // 1,0
        "    .incbin \"assets/maps/world-0002.ztm\"\n" // 2,0
        "    .incbin \"assets/maps/world-0003.ztm\"\n" // 3,0
        "    .incbin \"assets/maps/world-0004.ztm\"\n" // 4,0
        "    .incbin \"assets/maps/world-0005.ztm\"\n" // 5,0
        "    .incbin \"assets/maps/world-0006.ztm\"\n" // 6,0

        "    .incbin \"assets/maps/world-0016.ztm\"\n" // 0,1
        "    .incbin \"assets/maps/world-0017.ztm\"\n" // 1,1
        "    .incbin \"assets/maps/world-0018.ztm\"\n" // 2,1
        "    .incbin \"assets/maps/world-0019.ztm\"\n" // 3,1
        "    .incbin \"assets/maps/world-0020.ztm\"\n" // 4,1
        "    .incbin \"assets/maps/world-0021.ztm\"\n" // 5,1
        "    .incbin \"assets/maps/world-0022.ztm\"\n" // 6,1

        "    .incbin \"assets/maps/world-0032.ztm\"\n" // 0,2
        "    .incbin \"assets/maps/world-0033.ztm\"\n" // 1,2
        "    .incbin \"assets/maps/world-0034.ztm\"\n" // 2,2
        "    .incbin \"assets/maps/world-0035.ztm\"\n" // 3,2
        "    .incbin \"assets/maps/world-0036.ztm\"\n" // 4,2
        "    .incbin \"assets/maps/world-0037.ztm\"\n" // 5,2
        "    .incbin \"assets/maps/world-0038.ztm\"\n" // 6,2

        "    .incbin \"assets/maps/world-0048.ztm\"\n" // 0,3
        "    .incbin \"assets/maps/world-0049.ztm\"\n" // 1,3
        "    .incbin \"assets/maps/world-0050.ztm\"\n" // 2,3
        "    .incbin \"assets/maps/world-0051.ztm\"\n" // 3,3
        "    .incbin \"assets/maps/world-0052.ztm\"\n" // 4,3
        "    .incbin \"assets/maps/world-0053.ztm\"\n" // 5,3
        "    .incbin \"assets/maps/world-0054.ztm\"\n" // 6,3
        "__tilemap_end:\n"
    );
}