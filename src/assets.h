#include <stdint.h>
#include <zvb_gfx.h>
#include "world.h"


#define MAX_SPRITES   3

void __assets__(void);

extern gfx_sprite SPRITE_ARENA[MAX_SPRITES];

extern uint8_t _palette_end;
extern uint8_t _palette_start;
extern uint8_t _tileset_end;
extern uint8_t _tileset_start;

gfx_error load_palette(gfx_context* ctx);
gfx_error load_tileset(gfx_context* ctx, gfx_tileset_options* options);