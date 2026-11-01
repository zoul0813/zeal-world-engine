#include <zvb_gfx.h>
#include <zgdk/compress/rle.h>

#include "assets.h"
#include "tile.h"
#include "world/world.h"
#include "world_layout.h"
#include "chests.h"

static zar_file_t zar_file;
static zar_file_entry_t entry;
static uint8_t data[MAX_DATA_KB * 1024];

zos_err_t load_asset_file(const char *path) {
    zos_err_t err = zar_file_open(path, &zar_file);
    return err;
}

zos_err_t load_palette(gfx_context* ctx, const char *path, uint8_t from) {
    zos_err_t err = zar_file_entry_from_name(&zar_file, path, &entry);
    if(err != ERR_SUCCESS) return err | 0xC0;

    uint16_t size = entry.size;
    err = zar_file_read(&zar_file, &entry, data, &size);
    if(err != ERR_SUCCESS) return err | 0xD0;

    err = gfx_palette_load(ctx, data, size, from);
    if(err != ERR_SUCCESS) return err | 0x80;
    return err;
}

zos_err_t load_tileset(gfx_context* ctx, const char *path, gfx_tileset_options* options) {
    zos_err_t err = zar_file_entry_from_name(&zar_file, path, &entry);
    if(err != ERR_SUCCESS) return err | 0xC0;

    uint16_t size = entry.size;
    err = zar_file_read(&zar_file, &entry, data, &size);
    if(err != ERR_SUCCESS) return err | 0xC0;

    err = gfx_tileset_load_rle(ctx, data, size, options->from_byte, options->pal_offset, options->opacity);
    if(err != ERR_SUCCESS) return err | 0x80;
    return err;
}

zos_err_t load_tilemap(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t tile_x, uint8_t tile_y) {
    uint8_t offset = (y * WORLD_WIDTH) + x;
    zos_err_t err = zar_file_entry_from_index(&zar_file, TILEMAP_OFFSET + offset, &entry);
    if (err != ERR_SUCCESS) return err | 0xC0;

    uint16_t size = entry.size;
    uint8_t *rle_data = &data[(ROOM_WIDTH * ROOM_HEIGHT)];
    err = zar_file_read(&zar_file, &entry, rle_data, &size);
    if (err != ERR_SUCCESS) return err | 0xC0;

    decompress_rle(rle_data, size, data);

    uint8_t line[ROOM_WIDTH + 1];
    uint8_t has_unopened_chest = 0;

    for (uint8_t row = 0; row < ROOM_HEIGHT; row++) {
        for (uint8_t col = 0; col < ROOM_WIDTH; col++) {
            uint8_t tile = data[(row * ROOM_WIDTH) + col];
            if (tile == TILE_ID_CHEST && world_chest_open(x, y)) {
                tile = TILE_ID_CHEST_OPEN;
            }
            TileType type = tile_get_type(tile);
            tilemap_set_xy(col, row, type);
            if (type == TILE_CHEST) has_unopened_chest = 1;
            line[col] = tile;
        }

        err = gfx_tilemap_load(ctx, line, ROOM_WIDTH, LAYER0, tile_x, tile_y + row);
        if (err != ERR_SUCCESS) return err | 0x80;
    }

    ROOM_HAS_UNOPENED_CHEST = has_unopened_chest;

    return ERR_SUCCESS;
}

void room_set_tile(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t tile) {
    if (x >= ROOM_WIDTH || y >= ROOM_HEIGHT) return;

    Vector2_u8 origin;
    world_get_origin(&origin);
    gfx_tilemap_place(ctx, tile, LEVEL_LAYER, origin.x + ROOM_OFFSET_X + x, origin.y + ROOM_OFFSET_Y + y);
    tilemap_set_xy(x, y, tile_get_type(tile));
    if (tile == TILE_ID_CHEST_OPEN) world_open_chest();
}
