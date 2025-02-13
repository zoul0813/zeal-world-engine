#include <zgdk.h>

#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH   7
#define WORLD_HEIGHT  4
#define SCREEN_TILES  (20 * 15)

uint8_t world_transition(Edge edge);

#endif