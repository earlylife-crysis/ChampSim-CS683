#!/bin/bash

# Script to compile all necessary binaries

champsim_dir=/home/yashas/CS683/pa2-lehra-do-prefetcher-wronganswersonly

cd $champsim_dir

# Start compile (serially)

# (./build_champsim.sh no no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp2 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp4 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp5 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp6 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp8 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh no asp10 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_2mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_4mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_5mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_8mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_16mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh complex_stride_64mask no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh next_line no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh ip_stride no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_4phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_8phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_16phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_32phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_64phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_128phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_256phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_512phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_1024phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1
# (./build_champsim.sh optimized_2048phase no 1) >> $champsim_dir/scripts/build_log.txt 2>&1

(./build_champsim.sh optimized_32phase asp8 1) >> $champsim_dir/scripts/build_log.txt 2>&1