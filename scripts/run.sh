#!/bin/bash

# Bash script to run multiple benchmarks with different binaries

champsim_dir=".."
traces_dir="/home/yashas/benchmarks/qmm"
results_dir="logs"
binary="-hashed_perceptron-no-next_line-ip_stride-lru-1core"
arguments="-warmup_instructions 50000000 -simulation_instructions 100000000 -traces"

cd $champsim_dir

for trace_path in $traces_dir/*
do
trace=${trace_path##*/}
mkdir -p $results_dir/$trace
((bin/asp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-0threshold.txt 2>&1) &
((bin/asp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-disabled.txt 2>&1) &
((bin/asp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-enabled.txt 2>&1) &
((bin/dp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-0threshold.txt 2>&1) &
((bin/dp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-disabled.txt 2>&1) &
((bin/dp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-enabled.txt 2>&1) &
((bin/sp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-0threshold.txt 2>&1) &
((bin/sp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-disabled.txt 2>&1) &
((bin/sp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-enabled.txt 2>&1) &
((bin/no-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-0threshold.txt 2>&1) &
((bin/no-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-disabled.txt 2>&1) &
((bin/no-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-enabled.txt 2>&1) &
wait
done


