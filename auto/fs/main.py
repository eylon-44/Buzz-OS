# File System Builder

import sys
import os

from fs import FileSystem
from config import Config

def main() -> None:
    if len(sys.argv) < 2:
        raise Exception("Mount directory not specified.\nUsage: python3 fs.py path/to/mount")
    if not os.path.exists(sys.argv[1]):
        raise Exception(f"{sys.argv[1]} is not a valid path.")

    print("\n———————— FILE SYSTEM BUILD START ————————\n")
    Config.print()

    fs = FileSystem()
    fs.mount(sys.argv[1])

    print("\n———————— FILE SYSTEM BUILD END ————————\n")

if __name__ == '__main__':
    main()