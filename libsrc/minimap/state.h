#ifndef MINIMAP_STATE_H
#define MINIMAP_STATE_H

#include <zgdk/types.h>
#include <zos_errors.h>

/* State-only API: no ZVB calls or dependency on world runtime state. */
zos_err_t minimap_state_init(Vector2_u8 position);
zos_err_t minimap_state_set_position(Vector2_u8 position, uint8_t previous_is_special);
Vector2_u8 minimap_get_position(void);
uint8_t minimap_room_visited(uint8_t x, uint8_t y);
uint8_t minimap_room_is_special(uint8_t x, uint8_t y);

#endif
