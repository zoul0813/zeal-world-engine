#include "state.h"
#include "../world/layout.h"

#include <string.h>

#define MINIMAP_VISITED_SIZE ((WORLD_WIDTH * WORLD_HEIGHT) / 8)

static uint8_t minimap_visited[MINIMAP_VISITED_SIZE];
static uint8_t minimap_special[MINIMAP_VISITED_SIZE];
static Vector2_u8 minimap_position;

static uint8_t minimap_room_index(uint8_t x, uint8_t y) {
    return (y * WORLD_WIDTH) + x;
}

uint8_t minimap_room_visited(uint8_t x, uint8_t y) {
    if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return 0;
    uint8_t room = minimap_room_index(x, y);
    return minimap_visited[room >> 3] & (1 << (room & 7));
}

uint8_t minimap_room_is_special(uint8_t x, uint8_t y) {
    if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return 0;
    uint8_t room = minimap_room_index(x, y);
    return minimap_special[room >> 3] & (1 << (room & 7));
}

static void minimap_set_visited(uint8_t x, uint8_t y) {
    uint8_t room = minimap_room_index(x, y);
    minimap_visited[room >> 3] |= (1 << (room & 7));
}

static void minimap_set_special(uint8_t x, uint8_t y, uint8_t is_special) {
    uint8_t room = minimap_room_index(x, y);
    uint8_t bit = 1 << (room & 7);

    if (is_special) {
        minimap_special[room >> 3] |= bit;
    } else {
        minimap_special[room >> 3] &= ~bit;
    }
}

Vector2_u8 minimap_get_position(void) {
    return minimap_position;
}

zos_err_t minimap_state_init(Vector2_u8 position) {
    if (position.x >= WORLD_WIDTH || position.y >= WORLD_HEIGHT) return ERR_INVALID_PARAMETER;
    memset(minimap_visited, 0, sizeof(minimap_visited));
    memset(minimap_special, 0, sizeof(minimap_special));
    minimap_position = position;
    minimap_set_visited(position.x, position.y);
    return ERR_SUCCESS;
}

zos_err_t minimap_state_set_position(Vector2_u8 position, uint8_t previous_is_special) {
    if (position.x >= WORLD_WIDTH || position.y >= WORLD_HEIGHT) return ERR_INVALID_PARAMETER;
    minimap_set_visited(minimap_position.x, minimap_position.y);
    minimap_set_special(minimap_position.x, minimap_position.y, previous_is_special);
    minimap_position = position;
    minimap_set_visited(position.x, position.y);
    return ERR_SUCCESS;
}
