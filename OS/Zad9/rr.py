#!/usr/bin/env python3
import sys

def load_processes(csv_path):
    processes = []
    with open(csv_path, 'r') as f:
        for line in f:
            name, length, start = line.strip().split(',')
            processes.append([name, int(length), int(start), int(length)])
    return processes

def round_robin(csv_path, quantum):
    quantum = int(quantum)
    waiting = load_processes(csv_path)
    ready = []
    time = 0

    while waiting or ready:
        while waiting and waiting[0][2] <= time:
            p = waiting.pop(0)
            ready.append(p)
            print(f"T={time}: New process {p[0]} is waiting for execution (length={p[1]})")

        if not ready:
            if waiting:
                print(f"T={time}: No processes currently available")
                time = waiting[0][2]
                continue
            else:
                break

        p = ready.pop(0)
        run_time = min(quantum, p[3])

        print(f"T={time}: {p[0]} will be running for {run_time} time units. Time left: {p[3] - run_time}")

        p[3] -= run_time
        time += run_time

        if p[3] == 0:
            print(f"T={time}: Process {p[0]} has been finished")
        else:
            while waiting and waiting[0][2] <= time:
                new_p = waiting.pop(0)
                ready.append(new_p)
                print(f"T={time}: New process {new_p[0]} is waiting for execution (length={new_p[1]})")
            ready.append(p)

    print(f"T={time}: No more processes in queues")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python rr.py <csv_file> <quantum>")
        sys.exit(1)

    round_robin(sys.argv[1], sys.argv[2])