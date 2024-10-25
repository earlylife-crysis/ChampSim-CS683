#!/bin/bash

# Bash script to compile multiple binaries

# Unique identifier for binary
id="-enabled"

champsim_dir=".."

# Compile binaries for required prefetchers
$champsim_dir/./build_champsim.sh no $id
$champsim_dir/./build_champsim.sh sp_sbfp $id
$champsim_dir/./build_champsim.sh dp_sbfp $id
$champsim_dir/./build_champsim.sh asp_sbfp $id