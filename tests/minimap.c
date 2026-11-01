#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <minimap/minimap.h>
#include "support.h"

static uint8_t visited[20],special[20];
static minimap_state_config_t state={ .size={8,8},.visited=visited,.special=special,.capacity=8 };
static minimap_render_config_t renderer={ .position={38,0},.layer=1,.first_tile=200,.colors={2,3,4,5} };
static uint8_t pixel(uint8_t x,uint8_t y,uint8_t bpp) {
    uint16_t offset=(uint16_t)(x/16)*256+(uint16_t)y*16+x%16;
    if (bpp==8) return test_uploaded[offset];
    return offset&1 ? test_uploaded[offset/2]&15 : test_uploaded[offset/2]>>4;
}
int main(void) {
    gfx_context graphics={.bpp=4};
    assert(minimap_render(&graphics,&renderer)==GFX_INVALID_ARG);
    memset(visited,0xA5,sizeof(visited));memset(special,0xA5,sizeof(special));
    assert(minimap_init(&graphics,&state,&renderer,(Vector2_u8){2,3})==0);
    assert(test_uploaded_size==256 && test_uploaded_from==200*128);
    assert(test_placed_x[0]==38 && test_placed_x[1]==39 && test_placed_tile[1]==201 && test_placed_layer[0]==1);
    assert(visited[8]==0xA5 && special[8]==0xA5);
    assert(pixel(4,6,4)==5 && pixel(5,7,4)==5 && pixel(6,6,4)==2);
    assert(minimap_set_position(&graphics,(Vector2_u8){3,3},1)==0);
    assert(pixel(4,6,4)==4 && pixel(6,6,4)==5);
    assert(minimap_set_position(&graphics,(Vector2_u8){2,3},0)==0);
    assert(minimap_set_position(&graphics,(Vector2_u8){3,3},0)==0);
    assert(pixel(4,6,4)==3 && pixel(6,6,4)==5 && pixel(31,15,4)==2);
    assert(!minimap_room_is_special(2,3) && minimap_room_visited(2,3));
    assert(minimap_state_set_position((Vector2_u8){8,0},1)==ERR_INVALID_PARAMETER);
    Vector2_u8 before=minimap_get_position();assert(before.x==3 && before.y==3);
    assert(!minimap_room_visited(255,255));
    renderer.colors.current=16;assert(minimap_render(&graphics,&renderer)==GFX_INVALID_ARG);renderer.colors.current=5;
    state.size=(Size_u8){16,8};state.capacity=16;graphics.bpp=8;renderer.first_tile=253;renderer.position=(Vector2_u8){18,0};
    renderer.colors=(minimap_colors_t){23,27,36,16};
    assert(minimap_init(&graphics,&state,&renderer,(Vector2_u8){9,7})==0);
    assert(test_uploaded_size==512 && test_uploaded_from==253*256 && pixel(18,14,8)==16 && pixel(19,15,8)==16);
    state.size=(Size_u8){17,9};state.capacity=20;
    assert(minimap_state_init(&state,(Vector2_u8){16,8})==0 && minimap_room_visited(16,8));
    assert(minimap_render(&graphics,&renderer)==GFX_INVALID_ARG);
    state.capacity=19;assert(minimap_state_init(&state,(Vector2_u8){16,8})==ERR_INVALID_PARAMETER);
    assert(minimap_state_init(NULL,(Vector2_u8){0,0})==ERR_INVALID_PARAMETER);
    state.size=(Size_u8){8,8};state.capacity=8;
    test_graphics_error=GFX_FAILURE;
    assert(minimap_init(&graphics,&state,&renderer,(Vector2_u8){0,0})==GFX_FAILURE);
    assert(minimap_set_position(&graphics,(Vector2_u8){1,0},0)==GFX_INVALID_ARG);
    test_graphics_error=0;
    assert(minimap_init(&graphics,&state,&renderer,(Vector2_u8){0,0})==0);
    assert(minimap_init(NULL,&state,&renderer,(Vector2_u8){0,0})==GFX_INVALID_ARG);
    puts("PASS: runtime minimap dimensions/storage, generic markers, game colors and placement, 4/8-bit packing, validation and graphics errors");
    return 0;
}
