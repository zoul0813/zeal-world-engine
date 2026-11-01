#include "support.h"
#include <zgdk/tilemap.h>
#include <zgdk/tilemap/scroll.h>
#include <assert.h>
#include <string.h>

uint16_t test_scroll_x, test_scroll_y;
uint8_t test_scroll_layer;
unsigned test_scroll_count, test_wait_count, test_upload_count, test_place_count;
uint8_t test_uploaded[512];
uint16_t test_uploaded_size, test_uploaded_from;
uint8_t test_placed_tile[2], test_placed_x[2], test_placed_y[2], test_placed_layer[2];
uint8_t test_graphics_error;

void test_reset_graphics(void) {
    test_scroll_count = test_wait_count = test_upload_count = test_place_count = 0;
    test_graphics_error = 0;
}
void tilemap_scroll(uint8_t layer,uint16_t x,uint16_t y) {
    assert(layer <= 1 && x < 1280 && y < 640);
    test_scroll_layer=layer; test_scroll_x=x; test_scroll_y=y; test_scroll_count++;
}
gfx_error gfx_wait_vblank(gfx_context* ctx) { assert(ctx);test_wait_count++;return test_graphics_error; }
gfx_error gfx_wait_end_vblank(gfx_context* ctx) { assert(ctx);return test_graphics_error; }
gfx_error gfx_tileset_load(gfx_context* ctx,void* data,uint16_t size,const gfx_tileset_options* options) {
    assert(ctx && size<=sizeof(test_uploaded));
    memcpy(test_uploaded,data,size);test_uploaded_size=size;test_uploaded_from=options->from_byte;test_upload_count++;
    return test_graphics_error;
}
gfx_error gfx_tilemap_place(gfx_context* ctx,uint8_t tile,uint8_t layer,uint8_t x,uint8_t y) {
    assert(ctx && x<80 && y<40 && layer<2);
    unsigned n=test_place_count++ % 2;
    test_placed_tile[n]=tile;test_placed_x[n]=x;test_placed_y[n]=y;test_placed_layer[n]=layer;
    return test_graphics_error;
}
gfx_error gfx_tilemap_load(gfx_context* ctx,void* data,uint8_t size,uint8_t layer,uint8_t x,uint8_t y) {
    (void)data;assert(ctx && x+size<=80 && y<40 && layer<2);return test_graphics_error;
}
#ifndef TEST_USE_REAL_ZGDK
Tilemap* _TILEMAP;
zos_err_t tilemap_register(Tilemap* map,uint8_t* tiles) { _TILEMAP=map;map->tiles=tiles;return ERR_SUCCESS; }
int16_t tilemap_get_xy(uint8_t x,uint8_t y) {
    if (!_TILEMAP) return -1;
    if (x>=_TILEMAP->rect.w) return -2;
    if (y>=_TILEMAP->rect.h) return -3;
    return _TILEMAP->tiles[(uint16_t)y*_TILEMAP->rect.w+x];
}
int16_t tilemap_get(Point* point) {
    return tilemap_get_xy(point->x-_TILEMAP->rect.x,point->y-_TILEMAP->rect.y);
}
int16_t tilemap_set_xy(uint8_t x,uint8_t y,uint8_t tile) {
    if (tilemap_get_xy(x,y)<0) return -1;
    _TILEMAP->tiles[(uint16_t)y*_TILEMAP->rect.w+x]=tile;return tile;
}
#endif
