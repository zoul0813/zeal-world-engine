#ifndef MINIMAP_RENDER_H
#define MINIMAP_RENDER_H

#include <zvb_gfx.h>

#define MINIMAP_COLOR_CURRENT 16
#define MINIMAP_COLOR_VISITED 27
#define MINIMAP_COLOR_SPECIAL 36
#define MINIMAP_COLOR_UNVISITED 23

#define MINIMAP_TILE_LEFT 0xFD
#define MINIMAP_TILE_RIGHT 0xFE

/* Optional ZVB renderer for the minimap state. */
gfx_error minimap_render(gfx_context* ctx);
gfx_error minimap_place(gfx_context* ctx);

#endif
