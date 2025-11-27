#!/bin/bash

# Programy do kompresji:

#     gzip
#     bzip2
#     xz
#     zstd
#     lz4
#     7z

# Zadanie

# Napisz bashowy skrypt compr-bench, który jako argumenty przyjmuje katalogi, które będą kompresowane różnymi algorytmami.

# Działanie skryptu:

#     tworzy katalog tymczasowy przez polecenie tmp_dir=$(mktemp -d),
#     dla każdego katalogu z argumentów skryptu:
#         tworzy archiwum tar z jego zawartością (w katalogu tymczasowym), tar cvf "$tmp_dir/archive.tar" $1
#         wywołuje kolejno programy do kompresji i dekompresji mierząc czasy wykonywania i obliczając stosunek kompresji (rozmiaru pliku skompresowanego do oryginalnego) w procentach,
#         kasuje skompresowane archiwum i rozpakowane pliki oraz plik tar,
#     kasuje katalog tymczasowy razem z jego zawartością.

# Wypisz dane w kolumnach:

#     nazwa programu
#     czas kompresji
#     czas dekompresji
#     ratio



if [[ $# -lt 1 ]]; then
    echo "Script requires at least one folder to compress."
    exit 1
else
    tmp_dir=$(mktemp -d)

    for arg in "$@"; do
        tar cvf "$tmp_dir/archive.tar" $arg
        original_size=$(stat -c%s "$tmp_dir/archive.tar")
        printf "name" "compress" "decompress" "ratio"

        for compression_type in gzip bzip2 xz zstd lz4 7z; do
            start=`date +%s.%N`
            case $compression_type in
                gzip)
                    gzip "$tmp_dir/archive.tar"
                    compressed_file="$tmp_dir/archive.tar.gz"
                    ;;
            esac
            end=`date +%s.%N`
            compress_time=$(echo "$end - $start" | bc -l)

            compressed_size=$(stat -c%s "$compressed_file")

            start=`date +%s.%N`
            case $compression_type in
                gzip)
                    gunzip "$compressed_file"
                    ;;
            esac
            end=`date +%s.%N`
            decompress_time=$(echo "$end - $start" | bc -l)

            decompressed_size=$(stat -c%s "$tmp_dir/archive.tar")

            ratio=$(echo "$compressed_size / $original_size * 100" | bc)
        done
    done
    rm -rf "$tmp_dir"
fi