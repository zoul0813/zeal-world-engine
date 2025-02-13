#include "main.h"

#include <stdint.h>
#include <stdio.h>
#include <zgdk.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include "assets.h"
#include "player.h"

gfx_context vctx;

// uint8_t world_x = 0, world_y = 0, world_edge = EdgeNone;

int main(void) {
    // load_tilemap(get_tilemap_start(), WIDTH, HEIGHT, 0); exit(2);

    init();
reset:
    reset();
    clear_tilemap(0, 0, 80, 40, 0);  // clear all layer0 memory
    clear_tilemap(0, 0, 80, 40, 1);  // clear all layer1 memory

    load_tilemap(&vctx, 0, 0, 0, 0);
    // load_tilemap(&vctx, 1, 0, 1, 0);
    // load_tilemap(&vctx, 2, 0, 2, 0);
    // load_tilemap(&vctx, 3, 0, 3, 0);

    // load_tilemap(&vctx, 0, 1, 0, 1);
    // load_tilemap(&vctx, 1, 1, 1, 1);
    // load_tilemap(&vctx, 2, 1, 2, 1);
    // load_tilemap(&vctx, 3, 1, 3, 1);

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
        printf("\nError[%d] (%02x) %s", err, err, message);
        if (fatal) exit(err);
    }
}

void init(void) {
    zos_err_t err;
    err = input_init(1);
    handle_error(err, "failed to init keyboard", 1);
    input_flush();

    // disable the screen to prevent artifacts from showing
    gfx_enable_screen(0);

    sprites_register_arena(SPRITE_ARENA, MAX_SPRITES);

    err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_320_8BIT, &vctx);
    handle_error(err, "failed to init graphics", 1);

    err = load_palette(&vctx);
    handle_error(err, "failed to load palette", 1);

    gfx_tileset_options options = {
        .compression = TILESET_COMP_RLE,
    };
    err = load_tileset(&vctx, &options);
    handle_error(err, "failed to load tileset", 1);

    err = player_init();
    handle_error(err, "failed to init player", 1);

    gfx_enable_screen(1);
}

void deinit(void) {
    // reset screen
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);

    error err;
    err = player_deinit();
    handle_error(err, "failed to deinit player", 0);
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

    // if(LEFT1 && world_x > 0) world_x--;
    // if(RIGHT1 && world_x < (WORLD_WIDTH-1)) world_x++;
    // if(UP1 && world_y > 0) world_y--;
    // if(DOWN1 && world_y < (WORLD_HEIGHT - 1)) world_y++;

    if (SELECT1) return ACTION_QUIT;

    return 0;
}

void clear_tilemap(uint8_t x, uint8_t y, uint16_t width, uint16_t height, uint8_t layer) {
    uint8_t line[80];
    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            line[col] = 0xFF;
        }
        gfx_tilemap_load(&vctx, line, width, layer, x, y + row);
    }
}

void update(void) {
    Edge world_edge = player_move();
    if(world_edge != EdgeNone) {
        world_transition(world_edge);
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