#!/usr/bin/env python3

import argparse
from pathlib import Path

parser = argparse.ArgumentParser("gif2zeal")
parser.add_argument("-i", "--input", help="Input Folder", required=True)
parser.add_argument("-o", "--output", help="Output File", required=True)
parser.add_argument("-v", "--verbose", help="Verbose output", action='store_true')


def main():
  args = parser.parse_args()
  print("args", args)

if __name__ == "__main__":
  main()
