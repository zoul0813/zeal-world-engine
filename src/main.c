#include <stdint.h>
#include <core.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>
#include <zos_video.h>
#include <zgdk.h>
#include <zgdk/tilemap/scroll.h>

#include "main.h"
#include "assets.h"
#include "game_world.h"
#include "player.h"

gfx_context vctx;
gfx_sprite SPRITE_ARENA[MAX_SPRITES];
char buffer[WIDTH];

void format_u16_3(char* dest, uint16_t value) {
    itoa_pad(value, dest, 10, 'A', '0', 3);
}

void format_u16_3_pair(char* dest, uint16_t a, uint16_t b) {
    format_u16_3(dest, a);
    dest[3] = ' ';
    format_u16_3(dest + 4, b);
}

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
        put_s("\nError[");
        itoa(err, buffer, 10, 'A');
        put_s(buffer);
        put_s("] (");
        u8tohex(err, buffer, 'A');
        buffer[2] = '\0';
        put_s(buffer);
        put_s(") failed to ");
        put_s(message);
        put_c('\n');
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
        .from_byte = 0,
    };
    err = load_tileset(&vctx, "spritesh.zts", &options);
    handle_error(err, "load tileset", 1);

    gfx_tileset_add_color_tile(&vctx, 0xFF, 0); // create transparent/empty tile

    tilemap_fill(&vctx, LAYER0, EMPTY_TILE, 0, 0, 80, 40);
    tilemap_fill(&vctx, LAYER1, EMPTY_TILE, 0, 0, 80, 40);

    err = gfx_sprite_render_array(&vctx, 0, SPRITE_ARENA, MAX_SPRITES);
    handle_error(err, "clear sprites", 1);

    err = player_init();
    handle_error(err, "init player", 1);

    err = game_world_init(&vctx);
    handle_error(err, "initialize game world", 1);

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
    uint16_t input1 = input_read();

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

void update(void) {
    Edge screen_edge = player_move();
    if (screen_edge != EdgeNone) {
        zos_err_t err = game_world_transition(&vctx, screen_edge);
        handle_error(err, "change room", 1);
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
