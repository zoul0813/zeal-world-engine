#ifndef WORLD_TRANSITIONS_H
#define WORLD_TRANSITIONS_H

#include "world.h"

/* No entity positioning or drawing. Boundary/invalid queries return 0. */
uint8_t world_neighbor(Edge edge, Vector2_u8* destination);
/* 1 = entered neighbor, 0 = world boundary, negative = negated load/error code. */
int16_t world_move(Edge edge);

#endif
