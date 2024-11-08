#!/bin/bash

# Bash script to compile multiple binaries

# Unique identifier for binary
id="-noFP"

champsim_dir=".."
cd $champsim_dir

# Compile binaries for required prefetchers
# ./build_champsim.sh no $id
# ./build_champsim.sh sp_sbfp $id
# ./build_champsim.sh dp_sbfp $id
# ./build_champsim.sh asp_sbfp $id

./build_champsim.sh no $id
./build_champsim.sh asp_istlb $id
./build_champsim.sh sp_istlb $id
./build_champsim.sh markov_sota_istlb $id
./build_champsim.sh dp_istlb $id
# ./build_champsim.sh morriganPT $id

# ./build_champsim.sh no $id
# ./build_champsim.sh sp_sbfp $id
# ./build_champsim.sh dp_sbfp $id
# ./build_champsim.sh asp_sbfp $id
# ./build_champsim.sh agile $id