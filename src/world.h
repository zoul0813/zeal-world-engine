#include <zgdk.h>
#include "main.h"

#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH   7
#define WORLD_HEIGHT  4
#define SCREEN_TILES  (20 * 15)

extern Vector2_u8 world_position;

error world_init(gfx_context *ctx);
void screen_load(Vector2_u8 world, const Edge edge);
uint8_t screen_transition(Edge edge);

#endif