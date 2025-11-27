show_cpu() {
    cpu=$(grep "model name" /proc/cpuinfo | head -n1 | cut -d: -f2 | sed 's/^ *//')
    echo "CPU: $cpu"
}

show_ram() {
    ram=$(free -m | awk 'NR==2{printf "%s / %s MiB (%.2f%% used)\n", $3,$2,$3*100/$2 }')
    echo "RAM: $ram"
}

show_load() {
    load=$(awk '{ print $0 }' < /proc/loadavg)
    echo "Load: $load"
}

show_uptime() {
    uptime=$(uptime | awk -F'( |,|:)+' '{d=h=m=0; if ($7=="min") m=$6; else {if ($7~/^day/) {d=$6;h=$8;m=$9} else {h=$6;m=$7}}} {print d+0,"days,",h+0,"hours,",m+0,"minutes."}')
    echo "Uptime: $uptime"
}

show_kernel() {
    kernel=$(uname -r)
    echo "Kernel: $kernel"
}

show_gpu() {
    gpu=$(lspci | grep -iE 'VGA|3D|video' | awk -F': ' '{print $2}')
    echo "GPU: $gpu"
}

show_user() {
    user=$(whoami)
    echo "User: $user"
}

show_shell() {
    echo "Shell: $0"
}

show_processes() {
    processes=$(ps aux | wc -l)
    echo "Processes: $processes"
}

show_threads() {
    threads=$(ps -eo nlwp | tail -n +2 | awk '{ num_threads += $1 } END { print num_threads }')
    echo "Threads: $threads"
}

show_ip() {
    ip=$(ip -o -f inet addr show | awk '/scope global/ {print $4}' | tr '\n' ' ')
    echo "IP: $ip"
}

show_dns() {
    dns=$(resolvectl status | grep 'DNS Servers' | awk -F': ' '{print $2}')
    echo "DNS: $dns"
}

show_internet() {
    HOST=8.8.8.8

    ping -c1 $HOST 1>/dev/null
    SUCCESS=$?

    if [ $SUCCESS -eq 0 ]; then
        echo "Internet: OK"
    else
        echo "Internet: NOT OK"
    fi
}

show_all() {
    show_cpu
    show_ram
    show_load
    show_uptime
    show_kernel
    show_gpu
    show_user
    show_shell
    show_processes
    show_threads
    show_ip
    show_dns
    show_internet
}

if [[ $# -lt 1 ]]; then
    show_all
else
    for arg in "$@"; do
        case $arg in
            Cpu)
                show_cpu
                ;;
            Ram)
                show_ram
                ;;
            Load)
                show_load
                ;;
            Uptime)
                show_uptime
                ;;
            Kernel)
                show_kernel
                ;;
            Gpu)
                show_gpu
                ;;
            User)
                show_user
                ;;
            Shell)
                show_shell
                ;;
            Processes)
                show_processes
                ;;
            Threads)
                show_threads
                ;;
            Ip)
                show_ip
                ;;
            Dns)
                show_dns
                ;;
            Internet)
                show_internet
                ;;
            *)
                echo "Unknown argument: $arg"
                ;;
        esac
    done
fi