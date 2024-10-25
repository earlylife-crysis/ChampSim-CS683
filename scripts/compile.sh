#!/bin/bash

# Bash script to compile multiple binaries

# Unique identifier for binary
id="-enabled"

champsim_dir=".."
cd $champsim_dir

# Compile binaries for required prefetchers
./build_champsim.sh no $id
./build_champsim.sh sp_sbfp $id
./build_champsim.sh dp_sbfp $id
./build_champsim.sh asp_sbfp $id
