#ifndef TILE_H
#define TILE_H

#include <stdint.h>

typedef uint8_t TileType;

enum {
    TILE_OPEN = 0,         // player can move on this
    TILE_WALL = 1,         // wall, or other obstruction
    TILE_WATER = 2,        // water, an obstruction for now
    TILE_CHEST = 16,       // interactive chests
    TILE_CHEST_OPEN = 32,  // non-interactive chests
    TILE_UNKNOWN = 255,    // unknown, assume an obstruction
};

TileType tile_get_type(uint8_t tile);

#endif
