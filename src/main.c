#include <stdio.h>
#include <stdint.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>
#include <zos_video.h>
#include <zgdk.h>

#include "main.h"
#include "assets.h"
#include "world.h"
#include "player.h"

gfx_context vctx;
uint8_t buffer[WIDTH];

int main(void) {
    init();
reset:
    reset();

    while (true) {
        uint8_t action = input();
        if (action == ACTION_QUIT) goto quit_game;
        update();
        draw();
    }
quit_game:
    deinit();
    return 0;
}

void handle_error(zos_err_t err, const char* message, uint8_t fatal) {
    if (err != ERR_SUCCESS) {
        if (fatal) deinit();
        printf("\nError[%d] (%02x) failed to %s\n", err, err, message);
        if (fatal) exit(err);
    }
}

void init(void) {
    zos_err_t err;
    err = input_init(1);
    handle_error(err, "init keyboard", 1);
    input_flush();

    err = load_asset_file("world.zar");
    handle_error(err, "load zar", 1);

    sprites_register_arena(SPRITE_ARENA, MAX_SPRITES);

    ascii_map(' ', 1, 90);
    ascii_map('A', 16, 64);
    ascii_map('a', 16, 64);
    ascii_map('Q', 10, 80);
    ascii_map('q', 10, 80);
    ascii_map('0', 10, 96);

    // disable the screen to prevent artifacts from showing
    gfx_enable_screen(0);

    err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_320_8BIT, &vctx);
    handle_error(err, "init graphics", 1);

    err = load_palette(&vctx, "spritesh.ztp", 0);
    handle_error(err, "load palette", 1);

    err = gfx_tileset_add_color_tile(&vctx, EMPTY_TILE, 0);
    handle_error(err, "create empty tile", 1);

    gfx_tileset_options options = {
        .compression = TILESET_COMP_RLE,
    };
    err = load_tileset(&vctx, "spritesh.zts", &options);
    handle_error(err, "load tileset", 1);

    gfx_tileset_add_color_tile(&vctx, 0xFF, 0); // create transparent/empty tile

    clear_tilemap(LAYER0, EMPTY_TILE, 0, 0, 80, 40);  // clear all layer0 memory
    clear_tilemap(LAYER1, EMPTY_TILE, 0, 0, 80, 40);  // clear all layer1 memory

    err = player_init();
    handle_error(err, "init player", 1);

    err = world_init(&vctx);
    handle_error(err, "initialize world", 1);

    sprintf(buffer, "%03d %03d", world_position.x, world_position.y);
    nprint_string(&vctx, buffer, 7, 0, 0);

    gfx_enable_screen(1);
}

void deinit(void) {
    // reset the tilemap scroll
    tilemap_scroll(0, 0, 0);
    tilemap_scroll(1, 0, 0);

    // reset screen
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);

    error err;
    err = player_deinit();
    handle_error(err, "deinit player", 0);
}

void reset(void) {
    // reset your game state
}

static uint16_t input1_prev = 0;
uint8_t input(void) {
    uint16_t input1 = input_get();

    if (input1 == input1_prev) return 0;
    input1_prev = input1;

    player.direction.x = DIRECTION_NONE;
    player.direction.y = DIRECTION_NONE;
    if (LEFT1) player.direction.x = DIRECTION_LEFT;
    if (RIGHT1) player.direction.x = DIRECTION_RIGHT;
    if (UP1) player.direction.y = DIRECTION_UP;
    if (DOWN1) player.direction.y = DIRECTION_DOWN;

    if (SELECT1) return ACTION_QUIT;

    return 0;
}

void clear_tilemap(uint8_t layer, uint8_t tile, uint8_t x, uint8_t y, uint16_t width, uint16_t height) {
    uint8_t line[80];
    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            line[col] = tile;
        }
        gfx_error err = gfx_tilemap_load(&vctx, line, width, layer, x, y + row);
        handle_error(err, "clear tilemap", 1);
    }
}

void update(void) {
    Edge screen_edge = player_move();
    if (screen_edge != EdgeNone) {
        screen_transition(screen_edge, player.point, player_set_xy);
        sprintf(buffer, "%03d %03d", world_position.x, world_position.y);
        nprint_string(&vctx, buffer, 7, 0, 0);
    }
}

void draw_paused(uint8_t paused) {
    (void*)paused;  // unreferenced
}

void draw_gameover(uint8_t gameover) {
    (void*)gameover;  // unreferenced
}

void draw(void) {
    gfx_wait_vblank(&vctx);
    sprites_render(&vctx);
    gfx_wait_end_vblank(&vctx);
}
