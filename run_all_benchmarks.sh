#!/bin/sh

BENCHMARK_DIR=`pwd`/benchmark-out

for test in $(bazel query 'attr("tags", "benchmark", ...)'); do
    echo $test
    out_file="${BENCHMARK_DIR}$(echo $test | tr : / | tr -s / /).json" 
    mkdir -p $(dirname $out_file)
    bazel run -c opt $test -- \
	  --benchmark --benchmark_out=$out_file --benchmark_repetitions=5
	 
done
