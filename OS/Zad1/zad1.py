import builtins
from procfs import Proc
import os
import time

if not hasattr(builtins, 'basestring'):
    builtins.basestring = str

proc = Proc()

def get_process_info(pid):
    try:
        process = proc[pid]
        info = {
            'name': process.comm,
            'pid': pid,
            'cmdline': process.cmdline
        }
        return info
    except KeyError:
        return None

start_time = time.time()

pids = []
for name in os.listdir('/proc'):
    if name.isdigit():
        pids.append(int(name))

for counter, pid in enumerate(pids):
    info = get_process_info(pid)
    print(f"--- Iteration {counter} ---")
    for key, value in info.items():
        print(f"{key}: {value}")
    print("")

print("--- %s seconds ---" % (time.time() - start_time))