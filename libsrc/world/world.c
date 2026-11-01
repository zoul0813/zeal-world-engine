#include "world.h"
#include "internal.h"

#include <zgdk/tilemap/scroll.h>

Vector2_u8 world_position;
world_runtime_t world_runtime;

static uint8_t valid_config(const world_config_t* config, Vector2_u8 start) {
    if (config == NULL || config->tiles == NULL || config->load_room == NULL) return 0;
    if (!config->size.w || !config->size.h || start.x >= config->size.w || start.y >= config->size.h) return 0;
    if (!config->screen.w || !config->screen.h || config->screen.w % WORLD_TILE_PIXELS || config->screen.h % WORLD_TILE_PIXELS) return 0;
    uint16_t columns = config->screen.w / WORLD_TILE_PIXELS;
    uint16_t rows = config->screen.h / WORLD_TILE_PIXELS;
    if (columns > WORLD_MAP_COLUMNS || rows > WORLD_MAP_ROWS || config->layer > 1) return 0;
    if (!config->room.w || !config->room.h || config->room.x >= columns || config->room.y >= rows) return 0;
    if (config->room.w > columns - config->room.x || config->room.h > rows - config->room.y) return 0;
    if (config->tiles_size < config->room.w * config->room.h) return 0;
    if (config->transition != NULL && config->transition->run == NULL) return 0;
    return 1;
}

static zos_err_t load_visible(Vector2_u8 destination) {
    const world_config_t* config = &world_runtime.config;
    return config->load_room(world_runtime.ctx, destination.x, destination.y,
        world_runtime.origin.x + config->room.x, world_runtime.origin.y + config->room.y);
}

static zos_err_t present(void) {
    if (gfx_wait_vblank(world_runtime.ctx) != GFX_SUCCESS) return ERR_FAILURE;
    tilemap_scroll(world_runtime.config.layer,
        (uint16_t)world_runtime.origin.x * WORLD_TILE_PIXELS,
        (uint16_t)world_runtime.origin.y * WORLD_TILE_PIXELS);
    if (gfx_wait_end_vblank(world_runtime.ctx) != GFX_SUCCESS) return ERR_FAILURE;
    return ERR_SUCCESS;
}

static zos_err_t finish_entry(Vector2_u8 destination, Edge edge, zos_err_t err) {
    if (err == ERR_SUCCESS) err = present();
    if (err == ERR_SUCCESS) {
        world_position = destination;
        world_runtime.ready = 1;
        if (world_runtime.config.on_enter != NULL) {
            world_runtime.config.on_enter(world_runtime.ctx, destination.x, destination.y, edge);
        }
    } else {
        world_runtime.ready = 0;
    }
    world_runtime.busy = 0;
    return err;
}

zos_err_t world_init(gfx_context* ctx, const world_config_t* config, Vector2_u8 start) {
    if (world_runtime.busy) return ERR_FAILURE;
    if (ctx == NULL || !valid_config(config, start)) return ERR_INVALID_PARAMETER;

    Vector2_u8 origin = {0, 0};
    zos_err_t err = ERR_SUCCESS;
    world_runtime.busy = 1;
    if (config->transition != NULL && config->transition->prepare != NULL) {
        err = config->transition->prepare(config, &origin);
    }
    if (err == ERR_SUCCESS &&
        ((uint16_t)origin.x + config->screen.w / WORLD_TILE_PIXELS > WORLD_MAP_COLUMNS ||
         (uint16_t)origin.y + config->screen.h / WORLD_TILE_PIXELS > WORLD_MAP_ROWS)) {
        err = ERR_INVALID_PARAMETER;
    }
    if (err != ERR_SUCCESS) {
        world_runtime.busy = 0;
        return err;
    }

    world_runtime.ready = 0;
    world_runtime.ctx = ctx;
    world_runtime.config = *config;
    world_runtime.origin = origin;
    rect_set_pointer(&world_runtime.map.rect, config->room.x, config->room.y, config->room.w, config->room.h);
    err = tilemap_register(&world_runtime.map, config->tiles);
    if (err == ERR_SUCCESS) err = load_visible(start);
    return finish_entry(start, EdgeNone, err);
}

zos_err_t world_change_room(Vector2_u8 destination, Edge edge) {
    if (!world_runtime.ready || world_runtime.busy) return ERR_FAILURE;
    world_transition_request_t request;
    request.from = world_position;
    request.to = destination;
    request.origin = world_runtime.origin;
    request.edge = edge;
    world_runtime.busy = 1;
    zos_err_t err = ERR_SUCCESS;
    if (world_runtime.config.transition != NULL) {
        err = world_runtime.config.transition->run(world_runtime.ctx, &world_runtime.config, &request);
    }
    if (err == ERR_SUCCESS) err = load_visible(destination);
    return finish_entry(destination, edge, err);
}

const world_config_t* world_get_config(void) {
    return world_runtime.ready ? &world_runtime.config : NULL;
}

void world_get_origin(Vector2_u8* origin) {
    if (origin != NULL) *origin = world_runtime.origin;
}
