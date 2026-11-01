#include "chests.h"
#include "world/world.h"

#define WORLD_CHEST_STATE_SIZE ((WORLD_WIDTH * WORLD_HEIGHT) / 8)

uint8_t ROOM_HAS_UNOPENED_CHEST;
static uint8_t CHEST_OPEN[WORLD_CHEST_STATE_SIZE];

static uint8_t world_room_index(uint8_t x, uint8_t y) {
    return (y * WORLD_WIDTH) + x;
}

uint8_t world_chest_open(uint8_t x, uint8_t y) {
    if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return 0;
    uint8_t room = world_room_index(x, y);
    return CHEST_OPEN[room >> 3] & (1 << (room & 7));
}

static void world_set_chest_open(uint8_t x, uint8_t y) {
    uint8_t room = world_room_index(x, y);
    CHEST_OPEN[room >> 3] |= (1 << (room & 7));
}

void world_open_chest(void) {
    world_set_chest_open(world_position.x, world_position.y);
    ROOM_HAS_UNOPENED_CHEST = 0;
}

