#ifndef WORLD_SLIDE_H
#define WORLD_SLIDE_H

#include "world.h"

typedef struct {
    /* Optional game drawing during vblank, after each scroll step.
     * progress is the number of pixels scrolled in the exit direction. */
    void (*draw_frame)(gfx_context* ctx, Edge edge, uint16_t progress);
} world_slide_config_t;

/* Full-screen sliding, using three horizontal and two vertical staging screens.
 * prepare rejects layouts that cannot fit in ZVB's 80x40 tilemap.
 * Pass optional world_slide_config_t through config.transition_data. */
extern const world_transition_t WORLD_TRANSITION_SLIDE;

#endif
