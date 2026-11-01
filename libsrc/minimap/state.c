#include "state.h"

#include <string.h>

static minimap_state_config_t minimap_config;
static Vector2_u8 minimap_position;
static uint8_t ready;

static uint16_t room_index(uint8_t x, uint8_t y) {
    return (uint16_t)y * minimap_config.size.w + x;
}

static uint8_t room_valid(uint8_t x, uint8_t y) {
    return ready && x < minimap_config.size.w && y < minimap_config.size.h;
}

uint8_t minimap_room_visited(uint8_t x, uint8_t y) {
    if (!room_valid(x, y)) return 0;
    uint16_t room = room_index(x, y);
    return minimap_config.visited[room >> 3] & (1 << (room & 7));
}

uint8_t minimap_room_is_special(uint8_t x, uint8_t y) {
    if (!room_valid(x, y)) return 0;
    uint16_t room = room_index(x, y);
    return minimap_config.special[room >> 3] & (1 << (room & 7));
}

static void set_visited(uint8_t x, uint8_t y) {
    uint16_t room = room_index(x, y);
    minimap_config.visited[room >> 3] |= (1 << (room & 7));
}

Vector2_u8 minimap_get_position(void) {
    return minimap_position;
}

const minimap_state_config_t* minimap_get_config(void) {
    return ready ? &minimap_config : NULL;
}

zos_err_t minimap_state_init(const minimap_state_config_t* config, Vector2_u8 position) {
    if (config == NULL || config->visited == NULL || config->special == NULL || config->visited == config->special ||
        !config->size.w || !config->size.h || position.x >= config->size.w || position.y >= config->size.h ||
        config->capacity < MINIMAP_STATE_BYTES(config->size.w, config->size.h)) return ERR_INVALID_PARAMETER;
    minimap_config = *config;
    uint16_t bytes = MINIMAP_STATE_BYTES(config->size.w, config->size.h);
    memset(config->visited, 0, bytes);
    memset(config->special, 0, bytes);
    minimap_position = position;
    ready = 1;
    set_visited(position.x, position.y);
    return ERR_SUCCESS;
}

zos_err_t minimap_state_set_position(Vector2_u8 position, uint8_t previous_is_special) {
    if (!room_valid(position.x, position.y)) return ERR_INVALID_PARAMETER;
    uint16_t room = room_index(minimap_position.x, minimap_position.y);
    uint8_t bit = 1 << (room & 7);
    if (previous_is_special) minimap_config.special[room >> 3] |= bit;
    else minimap_config.special[room >> 3] &= ~bit;
    minimap_position = position;
    set_visited(position.x, position.y);
    return ERR_SUCCESS;
}
