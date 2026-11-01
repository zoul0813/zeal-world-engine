# world

Short description of the project

## Installation

## Building from source

Make sure that you have [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed.

Then open a terminal, go to the source directory and type the following commands:

```shell
    $ ./map2screen.py -i assets/world.tmx -m assets/maps/world.ztm -l 1 -s 20x15
    $ zde make
```

## Source layout

* `src/engine/` contains screen layout, room transitions, the registered ZGDK
  `Tilemap`, persistent room contents, and the minimap.
* The files directly in `src/` implement the test game: input, player movement,
  scoring, asset loading, graphics uploads, and tile-ID classification.

The game supplies its graphics context, starting room, and room-loader callback
to `world_init()`. The engine chooses which room to load and where to stage it
for scrolling. The game callback reads its archive, decompresses the map, uploads
its graphics, populates the registered map with `tilemap_set_xy()`, and reports
`ROOM_HAS_UNOPENED_CHEST`. Gameplay reads the map with ZGDK's `tilemap_get_xy()`;
the engine has no separate screen tile array or archive dependency.

Transitions report positions through a callback without depending on the player
module. Load errors propagate back to the game, which exits through its error
handler. Room content state (currently one opened-chest bit per room) belongs to
the engine; the game chooses the graphics and score for opening a chest. The
minimap generates its own UI tiles inside the engine.

Screen/world dimensions are still fixed in the engine headers.

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
