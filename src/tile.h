#ifndef TILE_H
#define TILE_H

#include <stdint.h>

#define TILE_ID_WALL        1
#define TILE_ID_WATER_START 16
#define TILE_ID_WATER_END   28
#define TILE_ID_STAIR_A     14
#define TILE_ID_STAIR_B     30
#define TILE_ID_CHEST       58
#define TILE_ID_CHEST_OPEN  59

typedef uint8_t TileType;

enum {
    TILE_OPEN = 0,         // traversable terrain
    TILE_WALL = 1,         // wall, or other obstruction
    TILE_WATER = 2,        // water, an obstruction for now
    TILE_CHEST = 16,       // interactive chests
    TILE_CHEST_OPEN = 32,  // non-interactive chests
    TILE_UNKNOWN = 255,    // unknown, assume an obstruction
};

TileType tile_get_type(uint8_t tile);

#endif
