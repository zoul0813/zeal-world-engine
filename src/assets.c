#include <stdio.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zvb_gfx.h>

#include "assets.h"

gfx_sprite SPRITE_ARENA[MAX_SPRITES];
uint8_t _palette_end;
uint8_t _palette_start;
uint8_t _tileset_end;
uint8_t _tileset_start;

gfx_error load_palette(gfx_context* ctx) {
    // Load the palette
    const size_t palette_size = &_palette_end - &_palette_start;
    return gfx_palette_load(ctx, &_palette_start, palette_size, 0);
}

gfx_error load_tileset(gfx_context* ctx, gfx_tileset_options* options) {
    const size_t size = &_tileset_end - &_tileset_start;
    return gfx_tileset_load(ctx, &_tileset_start, size, options);
}

// void __assets__(void) __naked {
//     INCLUDE_ASSET("palette", "assets/spritesheet.ztp");
//     INCLUDE_ASSET("tileset", "assets/spritesheet.zts");
// }

error load_asset_file(const char *path) {
    zos_dev_t fd = open(path, O_RDONLY);
    if(fd < 0) {
        printf("error %d [%02x]: loading %s\n", -fd, -fd, path);
        exit(-fd);
    }

    /**
     * ## ZGB Header
     * ZGB 0            "ZGB" {Version}
     * 0x01             {Number of Files, up to 255}
     *
     * ## File start/size
     * 0xFFFF 0xFFFF    {Start, Size}
     * 0xFFFF 0xFFFF    {Start, Size}
     * ......
     *
     * ## Data
     * ......
     */

    // size_t  POSITION_SIZE = 4; // 0xFFFF 0xFFFF
    // uint8_t palette_count = 1;
    // uint8_t sprite_count = 1;
    // uint8_t map_count = 128;
    // uint8_t track_count = 0;

    // uint8_t map_index = 2;

    // uint16_t map_start = (((palette_count + sprite_count) * POSITION_SIZE) + 8) + (POSITION_SIZE * 2);
    // uint16_t map_addr, map_size;
    // read(fd, &map_addr, sizeof(uint16_t));
    // read(fd, &map_size, sizeof(uint16_t));

    // seek(fd, map_addr, SEEK_SET);
    // read(fd, &map_data, &map_size);

    close(fd);
    return ERR_SUCCESS;
}