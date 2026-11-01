#ifndef WORLD_H
#define WORLD_H

#include <zgdk/types.h>
#include <zgdk/tilemap.h>
#include <zvb_gfx.h>
#include <zos_errors.h>

/* ZVB hardware geometry, independent of a game's viewport. */
#define WORLD_TILE_PIXELS 16
#define WORLD_MAP_COLUMNS 80
#define WORLD_MAP_ROWS 40

typedef struct world_config world_config_t;

/* A flat viewport rectangle supports static designated initializers in SDCC.
 * ZGDK's Rect has anonymous unions that SDCC mis-emits in such initializers. */
typedef struct {
    uint16_t x, y, w, h;
} world_viewport_t;

/* tile_x/tile_y are absolute tilemap coordinates, not screen indices.
 * Populate the registered ZGDK map with room-local logical tiles as well. */
typedef zos_err_t (*world_room_loader_t)(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t tile_x, uint8_t tile_y);
/* Called once after a successful entry, including initialization (EdgeNone).
 * It may draw game-owned content, but must not recursively change rooms. */
typedef void (*world_room_enter_t)(gfx_context* ctx, uint8_t x, uint8_t y, Edge edge);

typedef struct {
    Vector2_u8 from;
    Vector2_u8 to;
    Vector2_u8 origin;  /* stable viewport origin in tilemap tiles */
    Edge edge;
} world_transition_request_t;

typedef struct {
    /* Validate geometry and choose a stable viewport origin; no drawing. */
    zos_err_t (*prepare)(const world_config_t* config, Vector2_u8* origin);
    /* May stage/draw rooms. Must not commit identity or call on_enter.
     * The core reloads the destination at origin after this returns. */
    zos_err_t (*run)(gfx_context* ctx, const world_config_t* config, const world_transition_request_t* request);
} world_transition_t;

struct world_config {
    Size_u8 size;             /* world grid in rooms */
    Size screen;              /* visible screen in pixels */
    world_viewport_t room;    /* visible room rectangle in screen tiles */
    uint8_t layer;
    uint8_t* tiles;           /* caller-owned room.w * room.h logical tiles */
    uint16_t tiles_size;
    world_room_loader_t load_room;
    world_room_enter_t on_enter;
    const world_transition_t* transition; /* NULL = immediate replacement */
    const void* transition_data;          /* optional strategy-owned settings */
};

extern Vector2_u8 world_position;

/* Single active world, matching ZGDK's single registered Tilemap.
 * Config is copied; ctx, tiles, strategy and strategy data must remain alive.
 * Failed loads invalidate the active world: stop or reinitialize before reuse. */
zos_err_t world_init(gfx_context* ctx, const world_config_t* config, Vector2_u8 start);
const world_config_t* world_get_config(void);
void world_get_origin(Vector2_u8* origin);

#endif
