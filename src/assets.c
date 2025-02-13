#include <stdio.h>
#include <zvb_gfx.h>
#include <zgdk.h>
#include "assets.h"

gfx_sprite SPRITE_ARENA[MAX_SPRITES];

gfx_error load_palette(gfx_context* ctx) {
    // Load the palette
    const size_t palette_size = &_palette_end - &_palette_start;
    return gfx_palette_load(ctx, &_palette_start, palette_size, 0);
}

gfx_error load_tileset(gfx_context* ctx, gfx_tileset_options* options) {
    const size_t size = &_tileset_end - &_tileset_start;
    return gfx_tileset_load(ctx, &_tileset_start, size, options);
}

void load_tilemap(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y)
{
    uint8_t line[20];
    uint8_t* map = &tilemap[y][x][0];

    for (uint16_t row = 0; row < 15; row++) {
        uint16_t offset = row * 20;
        for (uint16_t col = 0; col < 20; col++) {
            uint8_t tile = map[offset + col];
            line[col] = tile;
        }
        gfx_tilemap_load(ctx, line, 20, 0, (map_x * 20), (map_y * 15) + row);
    }
}

void __assets__(void) __naked {
    INCLUDE_ASSET("palette", "assets/spritesheet.ztp");
    INCLUDE_ASSET("tileset", "assets/spritesheet.zts");
    // INCLUDE_ASSET("tilemap", "assets/maps/world-0.ztm");

    __asm__(
        "_tilemap:\n"
        "    .incbin \"assets/maps/world-0.ztm\"\n" // 0,0
        "    .incbin \"assets/maps/world-1.ztm\"\n" // 1,0
        "    .incbin \"assets/maps/world-2.ztm\"\n" // 2,0
        "    .incbin \"assets/maps/world-3.ztm\"\n" // 3,0
        "    .incbin \"assets/maps/world-4.ztm\"\n" // 4,0
        "    .incbin \"assets/maps/world-5.ztm\"\n" // 5,0
        "    .incbin \"assets/maps/world-6.ztm\"\n" // 6,0

        "    .incbin \"assets/maps/world-16.ztm\"\n" // 0,1
        "    .incbin \"assets/maps/world-17.ztm\"\n" // 1,1
        "    .incbin \"assets/maps/world-18.ztm\"\n" // 2,1
        "    .incbin \"assets/maps/world-19.ztm\"\n" // 3,1
        "    .incbin \"assets/maps/world-20.ztm\"\n" // 4,1
        "    .incbin \"assets/maps/world-21.ztm\"\n" // 5,1
        "    .incbin \"assets/maps/world-22.ztm\"\n" // 6,1

        "    .incbin \"assets/maps/world-32.ztm\"\n" // 0,2
        "    .incbin \"assets/maps/world-33.ztm\"\n" // 1,2
        "    .incbin \"assets/maps/world-34.ztm\"\n" // 2,2
        "    .incbin \"assets/maps/world-35.ztm\"\n" // 3,2
        "    .incbin \"assets/maps/world-36.ztm\"\n" // 4,2
        "    .incbin \"assets/maps/world-37.ztm\"\n" // 5,2
        "    .incbin \"assets/maps/world-38.ztm\"\n" // 6,2

        "    .incbin \"assets/maps/world-48.ztm\"\n" // 0,3
        "    .incbin \"assets/maps/world-49.ztm\"\n" // 1,3
        "    .incbin \"assets/maps/world-50.ztm\"\n" // 2,3
        "    .incbin \"assets/maps/world-51.ztm\"\n" // 3,3
        "    .incbin \"assets/maps/world-52.ztm\"\n" // 4,3
        "    .incbin \"assets/maps/world-53.ztm\"\n" // 5,3
        "    .incbin \"assets/maps/world-54.ztm\"\n" // 6,3
        "__tilemap_end:\n"
    );
}