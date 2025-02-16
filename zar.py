#!/usr/bin/env python3

import argparse
import os
import re
import struct
from pathlib import Path

parser = argparse.ArgumentParser("gif2zeal")
parser.add_argument("-i", "--input", help="Input Folder", required=True)
parser.add_argument("-o", "--output", help="Output File", required=False)
parser.add_argument("-v", "--verbose", help="Verbose output", action='store_true')
parser.add_argument("-x", "--extract", help="Extract Input to Output", action='store_true')
parser.add_argument("-l", "--list", help="List archive files", action='store_true')

FILE_HEADER_SIZE = 12 # uint16_t, uint16_t, char[8]

def generate_zar_filenames(filenames, max_length=8):
  seen = {}
  normalized = {}

  filtered_filenames = [file for file in filenames if not file.startswith(".")]

  for name in filtered_filenames:
    # Remove non-alphanumeric characters (optional)
    base, _ = os.path.splitext(os.path.basename(name))
    base = re.sub(r'[^a-zA-Z0-9]', '', base)

    short_name = base[:max_length]
    count = seen.get(short_name, 0)
    if count > 0:
      short_name = short_name[:max_length - 2] + str(count)
    seen[short_name] = count + 1

    short_name = short_name.ljust(max_length, '\x00')
    normalized[name] = short_name
  return normalized

def archive(args):
  if not args.output:
    print("Output filename required")
    return

  print("Archiving ", args.input, "to", args.output)
  rel_names = sorted(os.listdir(args.input))
  filenames = [os.path.abspath(os.path.join(args.input, file)) for file in rel_names]

  file_count = len(filenames)
  if file_count > 255:
    print("ZAR has a 255 file limit")
    return

  header = "ZAR"
  version = 0

  total_size = 0
  pointer = 0

  with open(args.output, "wb") as output:
    total_size += output.write(header.encode("ascii"))
    total_size += output.write(struct.pack("B", version))
    total_size += output.write(struct.pack("B", file_count))

    zar_files = generate_zar_filenames(filenames)

    pointer = total_size + (FILE_HEADER_SIZE * len(zar_files))
    for src, short_name in zar_files.items():
      size = os.path.getsize(src)
      short = short_name.encode("ascii")

      # start position of file
      total_size += output.write(struct.pack("H", pointer))
      # size of the file
      total_size += output.write(struct.pack("H", size))
      total_size += output.write(short)

      if args.verbose:
        print(pointer, size, short)

      # add the filesize to pointer
      pointer += size

    for src, short_name in zar_files.items():
      with open(src, "rb") as input:
        total_size += output.write(input.read())

def extract(args):
  if not args.list and not args.output:
    print("Output folder required")
    return

  print("Extracting ", args.input, "to", args.output)
  if not args.list:
    os.makedirs(args.output, exist_ok=True)

  with open(args.input, "rb") as input:
    header = input.read(3).decode("ascii")
    version = ord(input.read(1))
    file_count = ord(input.read(1))

    print(header, version, file_count)

    files = {}

    for i in range(file_count):
      data = input.read(2)
      pointer = struct.unpack("H", data)[0]
      data = input.read(2)
      size = struct.unpack("H", data)[0]
      data = input.read(8)
      short = data.decode("ascii").rstrip('\x00')

      files[short] = (pointer,size)

    for short,data in files.items():
      pointer,size = data
      input.seek(pointer)
      data = input.read(size)
      if args.verbose:
        print(short, pointer, size)
      if not args.list:
        with open(os.path.join(args.output, short), "wb") as output:
          output.write(data)



def main():
  args = parser.parse_args()
  print("args", args)

  if not args.extract and not args.list:
    archive(args)
  else:
    extract(args)


if __name__ == "__main__":
  main()
