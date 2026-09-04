#!/bin/bash

export DIRECTORY=$1 # parent directory

# Keep track of the current benchmark to print headers
prev_benchmark=""

# 1. Find files, extract metadata, and output: "benchmark config filepath"
find "$DIRECTORY" -type f -name "stats.txt" | while read -r file; do
    parent_dir=$(dirname "$file")
    grandparent_dir=$(dirname "$parent_dir")
    great_grandparent_dir=$(dirname "$grandparent_dir")
    
    config=$(basename "$great_grandparent_dir")
    bench_dir=$(basename "$grandparent_dir")
    benchmark="${bench_dir%%-*}"
    
    # Print out a space-separated string for sorting
    echo "$benchmark $config $file"

# 2. Sort the list alphabetically (groups by benchmark first, then config)
# 3. Read the sorted results and run the analysis
done | sort | while read -r benchmark config file; do

    # (Optional) Print a large divider when switching to a new benchmark
    if [ "$benchmark" != "$prev_benchmark" ]; then
        echo ""
        echo "################################################"
        echo "### STARTING BENCHMARK GROUP: $benchmark"
        echo "################################################"
        prev_benchmark="$benchmark"
    fi

    echo ""
    echo "====================="
    echo ""
    
    echo "Analyzing: $file"
    echo "  Configuration: $config"
    echo "  Benchmark $benchmark"
    
    ./bin/analyze "$file"
    echo ""
done
