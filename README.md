# world

Short description of the project

## Installation

## Building from source

Make sure that you have [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed.

Then open a terminal, go to the source directory and type the following commands:

```shell
    $ zde --no-tty cmake
```

## Source layout

* `libsrc/world/` contains world initialization and optional room transitions.
* `libsrc/minimap/` contains minimap state, optional ZVB rendering, and
  convenience functions combining the two.
* The files directly in `src/` implement the test game: input, player movement,
  scoring, chest persistence, asset loading, graphics uploads, and tile-ID classification.

The game supplies its graphics context, starting room, and room-loader callback
to `world_init()`. The engine chooses which room to load and where to stage it
for scrolling. The game callback reads its archive, decompresses the map, uploads
its graphics, and populates the registered map with `tilemap_set_xy()`. Gameplay
reads the map with ZGDK's `tilemap_get_xy()`;
the engine has no separate screen tile array or archive dependency.

Transitions report positions through a callback without depending on the player
module. Load errors propagate back to the game, which exits through its error
handler. The test game owns chest persistence in `src/chests.c`, including one
opened-chest bit per room and the current room's unopened-chest flag. The
minimap generates its own UI tiles inside the engine.

`libsrc/minimap/render.c` displays unvisited, visited, and special rooms, with a
separate highlight for the current room. The game supplies the special flag;
the test game uses it to mark rooms with unopened chests.

Screen/world dimensions are still fixed in the engine headers.

## Selecting engine features

The engine builds into `lib/world.lib`, with sources and headers in `libsrc/`.
The test game in `src/` links this archive using the same SDCC `-k`/`-l` options
as coreutils. Building the game also builds the library first. To build only
the library, run `zde --no-tty cmake --target world_engine`.

Both world and minimap features live in this single library. Each feature has
its own object file, so the linker selects only the modules a game actually
uses; no feature flags or separate libraries are needed.

| Feature | Header | Implementation |
| --- | --- | --- |
| World initialization and registered room map | `world/world.h` | `world/world.c` |
| Scrolling room transitions | `world/transitions.h` | `world/transitions.c` |
| Minimap state without graphics | `minimap/state.h` | `minimap/state.c` |
| ZVB minimap drawing and placement | `minimap/render.h` | `minimap/render.c` |
| Combined minimap initialization and updates | `minimap/minimap.h` | `minimap/minimap.c` |

Using only `world_init()` does not link transitions or the minimap.
Using only the minimap state API does not link its graphics buffer or renderer,
or any world runtime code. Both features share compile-time dimensions through
`libsrc/world/layout.h`; this header adds no runtime dependency.

Other games add `libsrc/` to their header search path and link with
`-k /path/to/zeal-world-engine/lib -l world.lib`, alongside ZGDK and the ZVB SDK.

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
