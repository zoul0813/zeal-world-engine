#ifndef UI_MAP_H
#define UI_MAP_H

#include <stdint.h>
#include <zvb_gfx.h>
#include <zgdk.h>

// #define UI_MAP_COLOR_CURRENT 31
#define UI_MAP_COLOR_CURRENT 16
#define UI_MAP_COLOR_VISITED 27
// #define UI_MAP_COLOR_UNOPENED_CHEST 16
#define UI_MAP_COLOR_UNOPENED_CHEST 36
#define UI_MAP_COLOR_BACKGROUND 23

#define UI_MAP_TILE_LEFT 0xFD
#define UI_MAP_TILE_RIGHT 0xFE

gfx_error ui_map_init(gfx_context* ctx, Vector2_u8 position);
gfx_error ui_map_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_has_unopened_chest);

#endif
