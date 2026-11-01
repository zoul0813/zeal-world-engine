#include "transitions.h"
#include "internal.h"

uint8_t world_neighbor(Edge edge, Vector2_u8* destination) {
    if (!world_runtime.ready || destination == NULL) return 0;
    Vector2_u8 next;
    next = world_position;
    switch (edge) {
        case EdgeLeft: if (!next.x) return 0; next.x--; break;
        case EdgeRight: if (next.x >= world_runtime.config.size.w - 1) return 0; next.x++; break;
        case EdgeTop: if (!next.y) return 0; next.y--; break;
        case EdgeBottom: if (next.y >= world_runtime.config.size.h - 1) return 0; next.y++; break;
        default: return 0;
    }
    *destination = next;
    return 1;
}

int16_t world_move(Edge edge) {
    if (edge != EdgeLeft && edge != EdgeRight && edge != EdgeTop && edge != EdgeBottom) return -ERR_INVALID_PARAMETER;
    if (!world_runtime.ready || world_runtime.busy) return -ERR_FAILURE;
    Vector2_u8 destination;
    if (!world_neighbor(edge, &destination)) return 0;
    zos_err_t err = world_change_room(destination, edge);
    return err == ERR_SUCCESS ? 1 : -(int16_t)err;
}
