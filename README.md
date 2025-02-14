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
