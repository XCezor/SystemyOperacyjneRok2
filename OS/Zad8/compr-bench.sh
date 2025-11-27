#!/bin/bash

create_temp_folder() {
    tmp_dir=$(mktemp -d)
}

if [[ $# -lt 2 ]]; then
    echo "Script requires at least one folder to compress."
    return 0
else do
    create_temp_folder

    for arg in "$@"; do
        tar cvf "$tmp_dir/archive.tar" $arg
    done
fi