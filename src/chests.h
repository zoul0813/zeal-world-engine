#ifndef GAME_CHESTS_H
#define GAME_CHESTS_H

#include <stdint.h>

/* Test-game chest state: one opened-chest bit per room. */
extern uint8_t ROOM_HAS_UNOPENED_CHEST;
uint8_t world_chest_open(uint8_t x, uint8_t y);
void world_open_chest(void);

#endif
