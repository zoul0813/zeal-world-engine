#include <zos_errors.h>
#include <zvb_gfx.h>
#include <zvb_sprite.h>
#include "player.h"
#include "main.h"

player_t player;

error player_init(void)
{
    player.point.x = SCREEN_WIDTH / 2;
    player.point.y = SCREEN_HEIGHT / 2;

    gfx_sprite sprite = {
        .tile = 58,
        .flags = SPRITE_NONE,
    };
    player.sprite_body = sprites_register(sprite);

    sprite.tile = 42;
    player.sprite_head = sprites_register(sprite);

    return GFX_SUCCESS;
}

error player_deinit(void)
{
    return ERR_SUCCESS;
}

Edge player_move(void)
{
    uint16_t x = player.point.x;
    uint16_t y = player.point.y;
    x          += (player.direction.x * PLAYER_SPEED);
    y          += (player.direction.y * PLAYER_SPEED);

    switch (player.direction.x) {
        case DIRECTION_LEFT: {
            player.sprite_head->flags = SPRITE_FLIP_X;
            player.sprite_body->flags = SPRITE_FLIP_X;
        } break;
        case DIRECTION_RIGHT: {
            player.sprite_head->flags = SPRITE_NONE;
            player.sprite_body->flags = SPRITE_NONE;
        } break;
    }

    player_set_xy(x, y);

    if(x < SPRITE_WIDTH) return EdgeLeft;
    else if(x > SCREEN_WIDTH) return EdgeRight;
    if(y < SPRITE_HEIGHT) return EdgeTop;
    else if(y > SCREEN_HEIGHT) return EdgeBottom;
    return EdgeNone;
}

void player_set_xy(uint16_t x, uint16_t y) {
    player.point.x = x;
    player.point.y = y;
    player.sprite_body->x = x;
    player.sprite_head->x = x;
    player.sprite_body->y = y;
    player.sprite_head->y = y - SPRITE_HEIGHT;
}
