#include <stdint.h>
#include <zvb_gfx.h>
#include <zar.h>

#define MAX_SPRITES    3
#define MAX_DATA_KB    16
#define TILEMAP_OFFSET 0

#define TILE_ID_PLAYER_HEAD 42
#define TILE_ID_PLAYER_BODY 43

#define TILE_ID_WALL        1
#define TILE_ID_WATER_START 16
#define TILE_ID_WATER_END   28
#define TILE_ID_STAIR_A     14
#define TILE_ID_STAIR_B     30
#define TILE_ID_CHEST       58
#define TILE_ID_CHEST_OPEN  59

extern gfx_sprite SPRITE_ARENA[MAX_SPRITES];
extern uint8_t data[MAX_DATA_KB * 1024];
extern zar_file_t zar_file;
extern zar_file_entry_t entry;

zos_err_t load_palette(gfx_context* ctx, const char *path, uint8_t from);
zos_err_t load_tileset(gfx_context* ctx, const char *path, gfx_tileset_options* options);
zos_err_t load_tilemap(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y);

zos_err_t load_asset_file(const char *path);
