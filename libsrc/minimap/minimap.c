#include "minimap.h"

static minimap_render_config_t render_config;
static uint8_t initialized;

gfx_error minimap_init(gfx_context* ctx, const minimap_state_config_t* state,
    const minimap_render_config_t* render, Vector2_u8 position) {
    if (ctx == NULL || render == NULL) return GFX_INVALID_ARG;
    initialized = 0;
    if (minimap_state_init(state, position) != ERR_SUCCESS) return GFX_INVALID_ARG;
    render_config = *render;
    gfx_error err = minimap_render(ctx, &render_config);
    if (err != GFX_SUCCESS) return err;
    err = minimap_place(ctx, &render_config);
    initialized = err == GFX_SUCCESS;
    return err;
}

gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special) {
    if (ctx == NULL || !initialized) return GFX_INVALID_ARG;
    if (minimap_state_set_position(position, previous_is_special) != ERR_SUCCESS) return GFX_INVALID_ARG;
    return minimap_render(ctx, &render_config);
}
