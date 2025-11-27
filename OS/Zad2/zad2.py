import time
import sys
import os

fileName = sys.argv[1]
counter = 0

if len(sys.argv) < 2:
    print("Poprawne użycie: python zad2.py \"file_name.txt\"")
    sys.exit(1)

with open(fileName, 'w') as file:
    while True:
        file.write(f"Linia {counter}\n")
        file.flush()
        os.fsync(file.fileno())
        counter += 1
        time.sleep(1)