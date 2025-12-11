#!/bin/bash

set -uo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <dir1> [dir2 ...]" >&2
    exit 1
fi

tmp_dir=$(mktemp -d)
trap 'rm -rf "$tmp_dir"' EXIT

compressors=(gzip bzip2 xz zstd lz4 7z)

print_table_header() {
    printf "name\tcompress\tdecompress\tratio\n"
}

missing_tool() {
    printf "%s\tskipped\tskipped\tskipped (not installed)\n" "$1"
}

for dir in "$@"; do
    if [[ ! -d "$dir" ]]; then
        echo "Skipping '$dir' (not a directory)." >&2
        continue
    fi

    archive="$tmp_dir/archive.tar"
    rm -f "$archive"
    tar -cf "$archive" -C "$dir" .
    original_size=$(stat -c%s "$archive")

    echo "$dir"
    print_table_header

    for compressor in "${compressors[@]}"; do
        compressed_file="$tmp_dir/archive.tar"

        case "$compressor" in
            gzip)
                compressed_file+=".gz"
                compress_cmd=(gzip -c "$archive")
                decompress_cmd=(gzip -dc "$compressed_file")
                ;;
            bzip2)
                compressed_file+=".bz2"
                compress_cmd=(bzip2 -c "$archive")
                decompress_cmd=(bzip2 -dc "$compressed_file")
                ;;
            xz)
                compressed_file+=".xz"
                compress_cmd=(xz -c "$archive")
                decompress_cmd=(xz -dc "$compressed_file")
                ;;
            zstd)
                compressed_file+=".zst"
                compress_cmd=(zstd -q -c "$archive")
                decompress_cmd=(zstd -q -d -c "$compressed_file")
                ;;
            lz4)
                compressed_file+=".lz4"
                compress_cmd=(lz4 -q -c "$archive")
                decompress_cmd=(lz4 -q -d -c "$compressed_file")
                ;;
            7z)
                compressed_file+=".7z"
                compress_cmd=(7z a -bd -t7z -mx=5 "$compressed_file" "$archive")
                decompress_cmd=(7z x -bd -so "$compressed_file")
                ;;
            *)
                continue
                ;;
        esac

        if ! command -v "$compressor" >/dev/null 2>&1; then
            missing_tool "$compressor"
            continue
        fi

        start=$(date +%s.%N)
        if [[ "$compressor" == "7z" ]]; then
            "${compress_cmd[@]}" >/dev/null
        else
            "${compress_cmd[@]}" >"$compressed_file"
        fi
        end=$(date +%s.%N)
        compress_time=$(echo "$end - $start" | bc -l)

        compressed_size=$(stat -c%s "$compressed_file")

        start=$(date +%s.%N)
        if [[ "$compressor" == "7z" ]]; then
            "${decompress_cmd[@]}" >/dev/null
        else
            "${decompress_cmd[@]}" >/dev/null
        fi
        end=$(date +%s.%N)
        decompress_time=$(echo "$end - $start" | bc -l)

        ratio=$(echo "scale=6; 100 * $compressed_size / $original_size" | bc -l)
        printf "%s\t%.9f\t%.9f\t%.1f%%\n" "$compressor" "$compress_time" "$decompress_time" "$ratio"

        rm -f "$compressed_file"
    done

    rm -f "$archive"
    echo
done