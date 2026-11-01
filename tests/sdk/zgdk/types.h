#ifndef TEST_ZGDK_TYPES_H
#define TEST_ZGDK_TYPES_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
typedef struct { uint8_t x,y; } Vector2_u8;
typedef struct { uint8_t w,h; } Size_u8;
typedef struct { uint16_t x,y; } Point;
typedef struct { uint16_t w,h; } Size;
/* Match ZGDK's union layout rather than a flat stand-in. */
typedef struct {
    union { struct { uint16_t x,y; }; Point point; };
    union { struct { uint16_t w,h; }; Size size; };
} Rect;
typedef enum { EdgeNone=0, EdgeTop=1, EdgeBottom=2, EdgeLeft=4, EdgeRight=8, EdgeContained=16 } Edge;
#define vec2_copy_pointer(dst,src) do { (dst)->x=(src)->x; (dst)->y=(src)->y; } while(0)
#define rect_set_pointer(rect,px,py,pw,ph) do { (rect)->x=(px); (rect)->y=(py); (rect)->w=(pw); (rect)->h=(ph); } while(0)
#endif
