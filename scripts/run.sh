#!/bin/bash

# Bash script to run multiple benchmarks with different binaries

champsim_dir=".."
traces_dir="/home/yashas/benchmarks/spec17"
results_dir="logs/agile_eval/spec17"
binary="-hashed_perceptron-no-next_line-ip_stride-lru-1core"
arguments="-warmup_instructions 250000000 -simulation_instructions 1000000000 -traces"

cd $champsim_dir

for trace_path in $traces_dir/*
do
trace=${trace_path##*/}
mkdir -p $results_dir/$trace
((bin/asp_sbfp-NaiveFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp-NaiveFP.txt 2>&1) &
((bin/asp_sbfp-NoFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp-NoFP.txt 2>&1) &
((bin/asp_sbfp-SBFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp-SBFP.txt 2>&1) &
((bin/dp_sbfp-NaiveFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp-NaiveFP.txt 2>&1) &
((bin/dp_sbfp-NoFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp-NoFP.txt 2>&1) &
wait
((bin/dp_sbfp-SBFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp-SBFP.txt 2>&1) &
((bin/sp_sbfp-NaiveFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp-NaiveFP.txt 2>&1) &
((bin/sp_sbfp-NoFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp-NoFP.txt 2>&1) &
((bin/sp_sbfp-SBFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp-SBFP.txt 2>&1) &
((bin/no-NaiveFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no-NaiveFP.txt 2>&1) &
wait
((bin/no-NoFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no-NoFP.txt 2>&1) &
((bin/no-SBFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no-SBFP.txt 2>&1) &
((bin/agile-NaiveFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/agile-NaiveFP.txt 2>&1) &
((bin/agile-NoFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/agile-NoFP.txt 2>&1) &
((bin/agile-SBFP$binary $arguments $traces_dir/$trace) > $results_dir/$trace/agile-SBFP.txt 2>&1) &

# ((bin/asp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/asp_istlb.txt 2>&1) &
# ((bin/dp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/dp_istlb.txt 2>&1) &
# ((bin/sp_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/sp_istlb.txt 2>&1) &
# ((bin/markov_sota_istlb-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/markov_sota_istlb.txt 2>&1) &
# ((bin/morriganPT-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/morriganPT.txt 2>&1) &
# ((bin/no-morrigan_eval$binary $arguments $traces_dir/$trace) > $results_dir/$trace/no.txt 2>&1) &
wait
done


