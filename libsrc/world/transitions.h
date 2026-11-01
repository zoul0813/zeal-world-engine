#ifndef WORLD_TRANSITIONS_H
#define WORLD_TRANSITIONS_H

#include "world.h"

typedef void (*transition_callback_t)(uint16_t x, uint16_t y);

zos_err_t screen_load(Vector2_u8 world, Edge edge, Point point, transition_callback_t callback);
/* Returns 1 after a transition, 0 at a world boundary, or a negated load error. */
int16_t screen_transition(Edge edge, Point point, transition_callback_t callback);

#endif
