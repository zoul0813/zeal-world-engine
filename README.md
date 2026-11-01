# world

A Zeal world library with game-owned room loading, runtime world/viewport
geometry, optional transitions, and an optional minimap. The game in `src/`
is a scrolling demo; the same `world.lib` also supports Enigma's larger rooms
and immediate room changes.

## Building

With [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed:

```sh
zde --no-tty cmake
```

This builds `lib/world.lib`, the demo binary, and its asset archive. To build
only the library: `zde --no-tty cmake --target world_engine`.

Other games add `libsrc/` to their include path and link with
`-k /path/to/zeal-world-engine/lib -l world.lib`, alongside ZGDK and the ZVB SDK.

## Ownership and layout

* `libsrc/world/world.c`: active world configuration, room-map registration,
  loading, and successful-entry notification.
* `libsrc/world/transitions.c`: neighbor selection and world-boundary checks.
* `libsrc/world/slide.c`: optional scrolling transition.
* `libsrc/minimap/`: independent minimap state and optional graphics rendering.
* `src/`: game-owned graphics/assets, tile classification, chest persistence,
  player positioning, input, scoring, and sprite animation.
  `src/game_world.c` owns the demo's world/minimap configuration and buffers,
  room-entry callbacks, and transition presentation. `src/main.c` coordinates
  game initialization, input, updates, and drawing.

The library contains no player, item, enemy, or tileset-specific behavior. It
uses ZGDK's single registered `Tilemap`, so one world is active at a time.
Each feature has a separate archive member: immediate transitions do not pull
in slide code, and state-only minimaps do not pull in graphics or world state.

## Configuring a world

`world_init(ctx, &config, start)` copies a `world_config_t`. The game supplies:

| Field | Meaning |
| --- | --- |
| `size` | World width and height in rooms |
| `screen` | Actual visible screen width and height in pixels |
| `room` | Visible room rectangle in screen tiles, including HUD offsets |
| `layer` | ZVB tilemap layer to scroll |
| `tiles`, `tiles_size` | Caller-owned logical room buffer and its byte capacity |
| `load_room` | Game-owned room loader |
| `on_enter` | Optional once-per-entry game callback |
| `transition` | `NULL` for immediate replacement, or a transition strategy |
| `transition_data` | Optional strategy-specific settings |

Tiles are ZVB's native 16x16 pixels. Screen dimensions must be multiples of 16;
room rectangles must fit the screen. The library validates geometry and buffer
capacity before calling the loader. Buffers, graphics context, strategy, and
strategy data must stay alive while the world is active.

For example, Enigma's layout needs no library rebuild:

```c
#include <world/world.h>
#include <world/transitions.h>

static uint8_t room_tiles[40 * 21];
static const world_config_t config = {
    .size = {8, 8},
    .screen = {640, 480},
    .room = {.x = 0, .y = 4, .w = 40, .h = 21},
    .layer = 0,
    .tiles = room_tiles,
    .tiles_size = sizeof(room_tiles),
    .load_room = load_room,
    .on_enter = room_entered,
    .transition = NULL,
};
```

The demo instead supplies a 16x8 world, 320x240 screen, 20x15 room at `(0,0)`,
and `&WORLD_TRANSITION_SLIDE`. Its fixed constants live in `src/world_layout.h`
and `src/screen.h`, outside the library.

## Loading and entering rooms

The loader signature is:

```c
zos_err_t load_room(gfx_context* ctx, uint8_t room_x, uint8_t room_y,
                    uint8_t tile_x, uint8_t tile_y);
```

`tile_x` and `tile_y` are **absolute tilemap coordinates**, already including
viewport offsets and transition staging. They are not screen indices. The game
reads/decompresses its map, uploads graphics at that origin, and populates the
registered logical map using room-local `tilemap_set_xy(col, row, type)` calls.
The loader must use the supplied room identity, not `world_position`, because
transitions may stage several rooms while the source remains current.

Call `world_move(edge)` to enter a neighbor. It returns `1` on success, `0` at a
world boundary, or a negated error code. `world_neighbor(edge, &destination)`
queries a neighbor without loading it. Games own movement, boundary clamping,
and arrival positions.

`on_enter(ctx, room_x, room_y, edge)` runs exactly once after all loading and
presentation succeed and `world_position` is committed. Initialization also
calls it, with `EdgeNone`. This is where an Enigma adapter can apply its room
palette, place its objects, and spawn its enemies. Those functions remain in
the game. Callbacks must not recursively change/reinitialize the world.

A failed load or presentation does not commit the destination or invoke
`on_enter`. Because the game loader may already have modified logical/visible
map data, the active world is invalidated; `world_get_config()` returns `NULL`
and further moves fail. The game must stop or successfully reinitialize before
continuing. Invalid configuration is rejected before replacing an active world.

## Optional transitions

With `transition = NULL`, the core loads once at the visible room rectangle,
keeps the viewport at tilemap origin `(0,0)`, and performs no animation.

The demo's slide is selected by including `world/slide.h` and assigning
`&WORLD_TRANSITION_SLIDE`. It requires a full-screen room and space for three
horizontal and two vertical staging screens in ZVB's 80x40 tilemap. Unsupported
layouts are rejected during initialization; Enigma's 640x480 layout uses the
immediate path.

`world_slide_config_t` optionally provides `draw_frame(ctx, edge, progress)`
through `transition_data`. It runs during vblank after each scroll step, with
progress in pixels. The game can animate/render its own visuals there. The
current demo keeps its short walk off screen and arrival positioning in game
code, preserving the existing slide without teaching the library about sprites.

A new transition can implement `world_transition_t`: optional `prepare()`
validates geometry and chooses the stable viewport origin without drawing;
`run()` stages/animates using the source, destination, edge, and origin in its
request. The core then reloads the destination at the stable origin, presents
it, commits identity, and notifies the game once.

## Optional minimap

`minimap/state.h` takes a world size and two caller-owned bitsets, each at least
`MINIMAP_STATE_BYTES(width, height)` bytes and non-overlapping. It tracks
unvisited, visited, and game-defined special rooms independently of the world
module. Updating position records the previous room's current special flag.

The optional two-tile renderer supports up to 16x8 rooms and 4-bit or 8-bit
graphics. `minimap_render_config_t` supplies placement, layer, two reserved tile
IDs, and palette indices for unvisited/visited/special/current. Four-bit colors
must be in `0..15`. The current-room color overlays the three room states.
Larger maps can reuse the state API with their own renderer.

The convenience API is `minimap_init(ctx, &state_config, &render_config, start)`
followed by `minimap_set_position(ctx, position, previous_is_special)`. The demo
uses its unopened-chest flag for "special"; the library has no chest knowledge.

## Validation

```sh
tests/run.sh
# Also exercise the installed ZGDK tilemap implementation:
ZGDK_PATH=/path/to/zeal-game-dev-kit tests/run.sh
# After building, compile/link both layouts against the same Z80 archive:
zde --no-tty exec python3 /src/tests/check-target.py
```

Host tests use AddressSanitizer/UndefinedBehaviorSanitizer and cover both
layouts, slide staging in four directions, one-time entry notifications,
boundaries, failure/reinitialization, reentrancy, configurable minimap storage,
and 4/8-bit rendering. Target probes inspect link maps to verify optional
modules remain unlinked. They are compile/link checks, not hardware runs.
The probes also check the emitted configuration bytes against SDCC's field
offsets, catching target-only static-initializer layout errors.

## Resources

* [Dungeon Tileset](https://opengameart.org/content/dungeon-tileset)

## Ideas

### HUD

* The HUD may consume any number of columns or rows on the screen
  * The screen map should account for this
  * If the HUD is 20x3 on the top (like Zelda)
  * The screen map should be 20x12

* Heart containers, like Zelda
  * This could be achieved with 4 tiles reserved
  * Using a graphic tile to represent 8x8 sprites
  * These sprites could then be "drawn" onto the four tiles as needed

* Minimap
  * The minimap could be represented by two tiles in memory
  * On a 16x8 world map
    * One of those tiles will have a 4x4px "dot"
    * The other tile will be "empty"
