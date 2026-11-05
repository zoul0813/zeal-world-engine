# Zeal world demo

A room-based scrolling game demonstrating ZGDK's rooms and independent minimap
modules. The reusable library now lives in
[zeal-game-dev-kit](https://github.com/zoul0813/zeal-game-dev-kit), under
`include/zgdk/rooms.h`, `include/zgdk/rooms/`, `include/zgdk/minimap.h`,
`include/zgdk/minimap/`, `src/rooms/` and `src/minimap/`.

## Build

With ZDE and an SDK containing the rooms/minimap modules:

```sh
zde --no-tty cmake
```

This builds the `world` demo and `world.zar` asset archive. It links `zgdk.lib`;
no separate `world.lib` or game-specific library include path is needed.

To use the sibling SDK clone before it is installed into ZDE, run from
`Projects/zeal` (so both checkouts are visible in the container):

```sh
zde --no-tty exec cmake -S /src/zeal-game-dev-kit -B /src/zeal-game-dev-kit/build/world-port
zde --no-tty exec cmake --build /src/zeal-game-dev-kit/build/world-port --parallel 4
zde --no-tty exec cmake --fresh -S /src/zeal-world-engine -B /src/zeal-world-engine/build -DZGDK_DIR=/src/zeal-game-dev-kit/cmake
zde --no-tty exec cmake --build /src/zeal-world-engine/build --parallel 4
```

`ZGDK_DIR` chooses the clone's CMake package instead of ZDE's managed SDK copy.

## Ownership

`src/game_world.c` supplies the 16x8 world, 320x240 viewport, room buffers,
loader, slide presentation and minimap configuration. Assets/decompression,
tile classifications, player movement and placement, chest persistence and
scoring remain game-owned. The SDK owns room navigation and validated entry,
optional slide staging and scrolling, and independent minimap state/rendering.

Use `<zgdk/rooms.h>`, `<zgdk/rooms/transitions.h>` and
`<zgdk/rooms/slide.h>` for room management. `<zgdk/minimap.h>` combines minimap
state and rendering; state-only users can include `<zgdk/minimap/state.h>`.

API documentation and validation now live with the implementation in ZGDK:

- [Rooms](https://github.com/zoul0813/zeal-game-dev-kit/blob/main/docs/Rooms.md)
- [Minimaps](https://github.com/zoul0813/zeal-game-dev-kit/blob/main/docs/Minimap.md)
- `sh tests/run-rooms.sh`: SDK host sanitizer checks using real tilemap functions.
- `python3 tests/check-rooms-target.py`: SDK SDCC ABI and optional-link checks.

The SDK tests exercise this demo's slide layout and Enigma's 640x480 immediate
room layout. Minimap-only users link no rooms code, and immediate rooms link no
slide implementation.

## Resources

- [Dungeon Tileset](https://opengameart.org/content/dungeon-tileset)
