#include "slide.h"

#include <zgdk/tilemap/scroll.h>

#define SLIDE_SPEED 4

static zos_err_t slide_prepare(const world_config_t* config, Vector2_u8* origin) {
    uint16_t columns = config->screen.w / WORLD_TILE_PIXELS;
    uint16_t rows = config->screen.h / WORLD_TILE_PIXELS;
    if (config->room.x || config->room.y || config->room.w != columns || config->room.h != rows ||
        columns * 3 > WORLD_MAP_COLUMNS || rows * 2 > WORLD_MAP_ROWS) return ERR_NOT_SUPPORTED;
    origin->x = columns;
    origin->y = 0;
    return ERR_SUCCESS;
}

static zos_err_t slide_run(gfx_context* ctx, const world_config_t* config, const world_transition_request_t* request) {
    uint8_t columns = config->room.w;
    uint8_t rows = config->room.h;
    int16_t scroll_x = (uint16_t)request->origin.x * WORLD_TILE_PIXELS;
    int16_t scroll_y = 0;
    int8_t dx = 0, dy = 0;
    uint16_t distance;
    zos_err_t err;
    switch (request->edge) {
        case EdgeLeft:
            dx = -1;
            distance = config->screen.w;
            err = config->load_room(ctx, request->to.x, request->to.y, 0, 0);
            break;
        case EdgeRight:
            dx = 1;
            distance = config->screen.w;
            err = config->load_room(ctx, request->to.x, request->to.y, columns * 2, 0);
            break;
        case EdgeTop:
            dy = -1;
            distance = config->screen.h;
            err = config->load_room(ctx, request->from.x, request->from.y, columns, rows);
            if (err != ERR_SUCCESS) return err;
            scroll_y = config->screen.h;
            if (gfx_wait_vblank(ctx) != GFX_SUCCESS) return ERR_FAILURE;
            tilemap_scroll(config->layer, scroll_x, scroll_y);
            if (gfx_wait_end_vblank(ctx) != GFX_SUCCESS) return ERR_FAILURE;
            err = config->load_room(ctx, request->to.x, request->to.y, columns, 0);
            break;
        case EdgeBottom:
            dy = 1;
            distance = config->screen.h;
            err = config->load_room(ctx, request->to.x, request->to.y, columns, rows);
            break;
        default: return ERR_INVALID_PARAMETER;
    }
    if (err != ERR_SUCCESS) return err;

    const world_slide_config_t* settings = config->transition_data;
    uint16_t progress = 0;
    while (progress < distance) {
        uint16_t step = distance - progress;
        if (step > SLIDE_SPEED) step = SLIDE_SPEED;
        progress += step;
        scroll_x += dx * (int16_t)step;
        scroll_y += dy * (int16_t)step;
        if (gfx_wait_vblank(ctx) != GFX_SUCCESS) return ERR_FAILURE;
        tilemap_scroll(config->layer, scroll_x, scroll_y);
        if (settings != NULL && settings->draw_frame != NULL) settings->draw_frame(ctx, request->edge, progress);
        if (gfx_wait_end_vblank(ctx) != GFX_SUCCESS) return ERR_FAILURE;
    }
    return ERR_SUCCESS;
}

const world_transition_t WORLD_TRANSITION_SLIDE = { slide_prepare, slide_run };
