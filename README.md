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

* `src/engine/` contains the reusable world code: screen layout, room loading,
  transitions, tile classification, persistent room contents, and the minimap.
* `src/main.c`, `src/main.h`, `src/player.c`, `src/player.h`, and `src/assets.h`
  implement the test game: input, player movement, sprite allocation, scoring,
  and the starting room and player position.

The game supplies its graphics context and starting room to `world_init()`.
Transitions report positions through a callback without depending on the player
module. Room content state (currently opened chests) belongs to the engine;
deciding when to open a chest and awarding its score belongs to the game.

The engine currently uses the bundled world's tile IDs and fixed screen/world
dimensions, defined in its headers. This separation identifies the reusable code
without introducing a configurable engine API yet.

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
