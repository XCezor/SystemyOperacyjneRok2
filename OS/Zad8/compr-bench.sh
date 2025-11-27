#!/bin/bash

if [[ $# -lt 1 ]]; then
    echo "Script requires at least one folder to compress."
    exit 1
else
    tmp_dir=$(mktemp -d)

    for arg in "$@"; do
        tar cvf "$tmp_dir/archive.tar" $arg
        echo "$tmp_dir/archive.tar" | gzip -c > plik.gz
    done
fi