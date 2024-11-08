#!/bin/bash

# Bash script to run multiple benchmarks with different binaries

champsim_dir=".."
traces_dir="/home/yashas/benchmarks/qmm"
results_dir="logs/morrigan_eval"
binary="-hashed_perceptron-next_line-next_line-spp_dev-lru-1core"
arguments="-warmup_instructions 50000000 -simulation_instructions 100000000 -traces"

cd $champsim_dir

for trace_path in $traces_dir/*
do
trace=${trace_path##*/}
mkdir -p $results_dir/$trace
# ((bin/asp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-0threshold.txt 2>&1) &
# ((bin/asp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-disabled.txt 2>&1) &
# ((bin/asp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_sbfp-enabled.txt 2>&1) &
# ((bin/dp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-0threshold.txt 2>&1) &
# ((bin/dp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-disabled.txt 2>&1) &
# ((bin/dp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_sbfp-enabled.txt 2>&1) &
# ((bin/sp_sbfp-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-0threshold.txt 2>&1) &
# ((bin/sp_sbfp-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-disabled.txt 2>&1) &
# ((bin/sp_sbfp-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_sbfp-enabled.txt 2>&1) &
# ((bin/no-0threshold$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-0threshold.txt 2>&1) &
# ((bin/no-disabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-disabled.txt 2>&1) &
# ((bin/no-enabled$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no_sbfp-enabled.txt 2>&1) &
# ((bin/asp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_istlb.txt 2>&1) &
# ((bin/dp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_istlb.txt 2>&1) &
# ((bin/sp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_istlb.txt 2>&1) &
# ((bin/markov_sota_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/markov_sota_istlb.txt 2>&1) &
# ((bin/morriganPT-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/morriganPT.txt 2>&1) &
((bin/no-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no.txt 2>&1) &
wait
done


