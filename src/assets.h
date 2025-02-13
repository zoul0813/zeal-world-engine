#include <stdint.h>
#include <zvb_gfx.h>
#include "world.h"


#define MAX_SPRITES   32

void __assets__(void);

extern gfx_sprite SPRITE_ARENA[MAX_SPRITES];

extern uint8_t _palette_end;
extern uint8_t _palette_start;
extern uint8_t _tileset_end;
extern uint8_t _tileset_start;
extern uint8_t tilemap[WORLD_HEIGHT][WORLD_WIDTH][SCREEN_TILES];

gfx_error load_palette(gfx_context* ctx);
gfx_error load_tileset(gfx_context* ctx, gfx_tileset_options* options);
void load_tilemap(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y);