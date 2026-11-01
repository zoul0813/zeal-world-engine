#include "minimap.h"

gfx_error minimap_init(gfx_context* ctx, Vector2_u8 position) {
    if (ctx == NULL) return GFX_INVALID_ARG;
    if (minimap_state_init(position) != ERR_SUCCESS) return GFX_INVALID_ARG;
    gfx_error err = minimap_render(ctx);
    if (err != GFX_SUCCESS) return err;
    return minimap_place(ctx);
}

gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special) {
    if (ctx == NULL) return GFX_INVALID_ARG;
    if (minimap_state_set_position(position, previous_is_special) != ERR_SUCCESS) return GFX_INVALID_ARG;
    return minimap_render(ctx);
}
