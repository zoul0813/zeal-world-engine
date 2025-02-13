#include <zgdk.h>

#include "assets.h"
#include "main.h"
#include "player.h"

Vector2_u8 world_position;
// uint8_t world_x = 0, world_y = 0, edge = EdgeNone;

uint8_t world_transition(Edge edge) {
    uint8_t transition = 0;
    switch (edge) {
        case EdgeLeft:
            if (world_position.x > 0) {
                world_position.x--;
                player_set_xy(SCREEN_WIDTH, player.point.y);
                transition=1;
            } else {
                player_set_xy(SPRITE_WIDTH, player.point.y);
            }
            break;
        case EdgeRight:
            if (world_position.x < (WORLD_WIDTH - 1)) {
                world_position.x++;
                player_set_xy(SPRITE_WIDTH, player.point.y);
                transition=1;
            } else {
                player_set_xy(SCREEN_WIDTH, player.point.y);
            }
            break;
        case EdgeTop:
            if (world_position.y > 0) {
                world_position.y--;
                player_set_xy(player.point.x, SCREEN_HEIGHT);
                transition=1;
            } else {
                player_set_xy(player.point.x, SPRITE_HEIGHT);
            }
            break;
        case EdgeBottom:
            if (world_position.y < (WORLD_HEIGHT - 1)) {
                world_position.y++;
                player_set_xy(player.point.x, SPRITE_HEIGHT);
                transition=1;
            } else {
                player_set_xy(player.point.x, SCREEN_HEIGHT);
            }
            break;
    }
    if(transition) {
        Point pos = {
            .x = player.point.x,
            .y = player.point.y,
        };

        // hide the player
        player_set_xy(0, 0);
        gfx_wait_vblank(&vctx);
        sprites_render(&vctx);
        gfx_wait_end_vblank(&vctx);
        msleep(250); // brief delay

        // render the new screen
        gfx_wait_vblank(&vctx);
        load_tilemap(&vctx, world_position.x, world_position.y, 0, 0);
        gfx_wait_end_vblank(&vctx);
        msleep(250); // brief delay

        // show the player
        player_set_xy(pos.x, pos.y);
        gfx_wait_vblank(&vctx);
        sprites_render(&vctx);
        gfx_wait_end_vblank(&vctx);
        msleep(250); // brief delay

        return 1;
    }
    return 0;
}