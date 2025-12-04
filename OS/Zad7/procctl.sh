getCPU() {
    echo "Top 5 processes by CPU usage:"
    ps aux | sort -nrk 3,3 | head -n 5
}

getMemory() {
    echo "Top 5 processes by memory usage:"
    ps aux | sort -nrk 4,4 | head -n 5
}

getProcessTree() {
    echo "Process tree:"
    pstree -p
}

getProcessPID() {
    read -p "PID: " pid
    pname=$(ps -p $pid -o comm=)
    if [ -n "$pname" ]; then
        echo "PID $pid: $pname"
    else
        echo "No process found with PID $pid"
    fi
}

getProcessPIDByName() {
    read -p "Process name: " pname
    pids=$(pgrep $pname)
    if [ -n "$pids" ]; then
        echo "Name '$pname':"
        echo "$pids"
    else
        echo "No process found with name '$pname'"
    fi
}

killProcessPID() {
    read -p "PID to kill: " pid
    kill $pid && echo "Process with PID $pid killed." || echo "Failed to kill process with PID $pid."
}

killProcessName() {
    read -p "Process name to kill: " pname
    pkill $pname && echo "Processes with name '$pname' killed." || echo "Failed to kill processes with name '$pname'."
}

echo "Process Control:"
echo "1) List top 5 processes by CPU usage"
echo "2) List top 5 processes by memory usage"
echo "3) Show process tree"
echo "4) Show process name by PID"
echo "5) Show process PID(s) by name"
echo "6) Kill process by PID"
echo "7) Kill process by name"
echo "q) Exit"
read -p "Choice: " choice

case $choice in
    1)
        getCPU
        ;;
    2)
        getMemory
        ;;
    3)
        getProcessTree
        ;;
    4)
        getProcessPID
        ;;
    5)
        getProcessPIDbyName
        ;;
    6)
        killProcessPID
        ;;
    7)
        killProcessName
        ;;
    q)
        echo "Exiting."
        ;;
    *)
        echo "Invalid choice."
        ;;
esac