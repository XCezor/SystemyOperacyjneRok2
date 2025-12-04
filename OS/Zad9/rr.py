#!/usr/bin/env python3
import sys

def main(*args):
    print("Arguments received:", args)
    csv_path = args[0]
    with open(csv_path, 'r') as file:
        content = file.read()
        print(f"File content:\n{content}")

if len(sys.argv) != 3:
    print("Usage: /rr [\"name\", length, start], length2")
else:
    main(sys.argv[1], sys.argv[2])