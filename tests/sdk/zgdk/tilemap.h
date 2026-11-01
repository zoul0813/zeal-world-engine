#ifndef TEST_ZGDK_TILEMAP_H
#define TEST_ZGDK_TILEMAP_H
#include <zgdk/types.h>
#include <zvb_gfx.h>
#include <zos_errors.h>
typedef struct { uint8_t* tiles; Rect rect; } Tilemap;
extern Tilemap* _TILEMAP;
zos_err_t tilemap_register(Tilemap*,uint8_t*);
int16_t tilemap_get(Point*);
int16_t tilemap_get_xy(uint8_t,uint8_t);
int16_t tilemap_set_xy(uint8_t,uint8_t,uint8_t);
#endif
