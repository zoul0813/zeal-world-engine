#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include <zvb_gfx.h>
#include <zar.h>

#define MAX_SPRITES 3
#define MAX_DATA_KB 16
#define TILEMAP_OFFSET 0
#define TILE_ID_PLAYER_HEAD 42
#define TILE_ID_PLAYER_BODY 43

extern gfx_sprite SPRITE_ARENA[MAX_SPRITES];

zos_err_t load_asset_file(const char *path);
zos_err_t load_palette(gfx_context *ctx, const char *path, uint8_t from);
/* The demo's spritesheet is RLE encoded. compression is not used. */
zos_err_t load_tileset(gfx_context *ctx, const char *path, gfx_tileset_options *options);
zos_err_t room_load(gfx_context *ctx, uint8_t x, uint8_t y, uint8_t tile_x, uint8_t tile_y);
void room_set_tile(gfx_context *ctx, uint8_t x, uint8_t y, uint8_t tile);

#endif
