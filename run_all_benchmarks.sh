#!/bin/sh

BENCHMARK_DIR=`pwd`/benchmark-out

BENCHMARK_QUERY='attr("tags", "benchmark", ...) except attr("tags", "slowbenchmark", ...)'

BENCHMARK_FLAGS="--benchmark --benchmark_repetitions=5"
BENCHMARK_FLAGS="--benchmark"

rm -r $BENCHMARK_DIR

for test in $(bazel query $BENCHMARK_QUERY); do
    echo $test
    out_file="${BENCHMARK_DIR}$(echo $test | tr : / | tr -s / /).json" 
    mkdir -p $(dirname $out_file)
    bazel run -c opt $test -- ${BENCHMARK_FLAGS} --benchmark_out=$out_file
done
