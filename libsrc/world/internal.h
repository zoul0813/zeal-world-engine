#ifndef WORLD_INTERNAL_H
#define WORLD_INTERNAL_H

#include "world.h"

typedef struct {
    gfx_context* ctx;
    world_config_t config;
    Tilemap map;
    Vector2_u8 origin;
    uint8_t ready;
    uint8_t busy;
} world_runtime_t;

extern world_runtime_t world_runtime;
zos_err_t world_change_room(Vector2_u8 destination, Edge edge);

#endif
