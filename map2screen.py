#!/usr/bin/env python3

import argparse
from pathlib import Path
import xml.etree.ElementTree as ET

##
# <map version="1.10" tiledversion="1.11.2" orientation="orthogonal" renderorder="right-down" width="320" height="128" tilewidth="16" tileheight="16" infinite="0" nextlayerid="3" nextobjectid="1">
# <layer id="2" name="Background" width="320" height="128" locked="1"></layer>
# <layer id="1" name="Room 1" width="320" height="128">
#   <data encoding="csv">
#     2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,16
#     5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,16
#     5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,16
#     5,1,1,1,1,1,1,1,1,1,10,8,8,8,8,11,1,1,6,16
#     5,1,1,1,1,1,1,1,10,8,9,18,19,19,20,5,1,1,6,16
#     5,1,1,1,1,1,1,10,9,18,19,29,26,24,25,5,1,1,1,1
#     5,1,1,1,1,1,10,9,18,29,26,24,25,2,3,13,1,1,1,1
#     7,8,8,8,8,8,9,18,29,26,25,2,3,13,1,1,1,1,1,1
#     19,19,19,19,19,19,19,29,26,25,2,13,1,1,1,1,1,1,1,18
#     17,53,26,24,24,24,24,24,25,2,13,1,1,1,1,1,1,1,6,38
#     17,26,25,2,3,3,3,3,3,13,1,1,1,1,1,1,1,1,6,38
#     26,25,2,13,1,1,1,1,1,1,1,1,1,1,1,1,1,10,9,38
#     22,2,13,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,18,22
#     22,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,6,21,28
#     22,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,21,17
#   </data>
# </layer>
##

class DimensionAction(argparse.Action):
  def __init__(self, option_strings, dest, nargs=None, **kwargs):
    if nargs is not None:
      raise ValueError("nargs not allowed")
    super().__init__(option_strings, dest, **kwargs)
  def __call__(self, parser, namespace, values, option_string=None):
    raw = values.split("x")
    dimension = {
      "width": int(raw[0]),
      "height": int(raw[1]),
    }
    setattr(namespace, self.dest, dimension)


parser = argparse.ArgumentParser("gif2zeal")
parser.add_argument("-i", "--input", help="Input GIF Filename", required=True)
parser.add_argument("-m", "--tilemap", help="Zeal Tilemap (ZTM)")
parser.add_argument("-l", "--layer", help="Tiled Layer to yse", type=int)
parser.add_argument("-s", "--size", help="Screen Width/Height", action=DimensionAction, default=None)
parser.add_argument("-v", "--verbose", help="Verbose output", action='store_true')

def get_screen(layer):
  pass

def convert(args):
  maps = []
  tree = ET.parse(args.input)
  root = tree.getroot()

  meta = root.attrib
  print("meta", meta)

  layers = None
  if args.layer:
    layers = root.findall("layer[@id='" + str(args.layer) + "']")
  else:
    layers = root.findall("layer")
  for layer in layers:
    data = layer.find("data")

    if not data.attrib["encoding"] == 'csv':
      print("Invalid Data Encoding, expected CSV and got ", data.attrib["encoding"])
      return None

    # tiled indexes are 1 based, so we subtract 1 to make it 0 based
    tiles = [255 if int(i) < 1 else int(i) - 1 for i in data.text.replace("\n", "").split(",")]
    print("tiles", len(tiles))
    if args.size:
      mw = int(meta["width"])       # map width
      mh = int(meta["height"])      # map height
      sw = int(args.size["width"])  # screen width
      sh = int(args.size["height"]) # screen height
      sx = mw//sw                   # horizontal screens
      sy = mh//sh                   # vertical screens

      print("getting screens", args.size, "screen w/h", sw, sh, "map w/h", mw, mh, "total", sx, sy)

      for i in range(sy): # vertical screens
        for j in range(sx): # horizontal screens
          world_offset = ((mw * i) * sh) + (sw * j)
          screen = []
          for y in range(sh): # screen height
            line = []
            for x in range(sw): # screen width
              offset = world_offset + (mw * y) + x
              tile = tiles[offset]
              if args.verbose:
                print("offset", {
                  "map_y": i,
                  "map_x": j,
                  "scr_y": y,
                  "scr_x": x,
                  "offset": offset,
                  "tile": tile
                })
              line.append(tile)
            screen = screen + line
          if args.verbose:
            print("screen", len(screen), sh, sw)
          maps.append(bytes(screen))

    else:
      maps.append(bytes(tiles))

  return maps

def main():
  args = parser.parse_args()
  print("args", args)
  if args.size:
    print("screen dimensions", args.size["width"], args.size["height"])
  maps = convert(args)
  if not maps or len(maps) < 1:
    print("Failed to convert")
    return

  for idx, tilemap in enumerate(maps):
    if args.tilemap:
      p = Path(args.tilemap)
      index = ""
      if len(maps) > 1:
        index = f"-{str(idx).zfill(4)}"
      tilemapFileName = p.parent / f"{p.stem}{index}{p.suffix}"

    if tilemapFileName == None:
      tilemapFileName = Path(args.input + "-" + str(idx).zfill(4)).with_suffix(".ztm")

    if args.verbose:
      print("tilemap", tilemapFileName) #, tilemap)
    with open(tilemapFileName, "wb") as file:
      file.write(tilemap)


if __name__ == "__main__":
  main()
