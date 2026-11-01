#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <zvb_gfx.h>
#include <zos_errors.h>
#include <zgdk/types.h>

/* Demo-specific world setup, room presentation, and minimap integration. */
zos_err_t game_world_init(gfx_context* ctx);
zos_err_t game_world_transition(gfx_context* ctx, Edge edge);

#endif
