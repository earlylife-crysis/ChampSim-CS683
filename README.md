[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/2a23ZfC_)
<p align="center">
  <h1 align="center"> ChampSim </h1>
  <p> ChampSim is a trace-based simulator for a microarchitecture study. You find the required trace files at (https://drive.google.com/drive/folders/1JmFmXRZ1A517KtmiZvpBwobfb79dk_GM?usp=sharing) <p>
</p>

## Tested Environment & Dependencies

- Ubuntu 18.04.6 LTS and above
- Linux Kernel 5.4.0 and above
- GCC 7.5.0

# Compile

To makes things simper, you are only required to specify three parameters: L1D prefetcher, STLB prefetcher, and the number of cores. 
For example, `./build_champsim.sh no no 1` builds a single-core processor with no L1 data prefetcher or STLB prefetcher.
```
$ ./build_champsim.sh ip_stride asp 1

$ ./build_champsim.sh ${L1D_PREFETCHER} ${STLB_PREFETCHER} ${NUM_CORE}
```

In case you need to modify the L2C/LLC prefetchers, you can manually change it in `./build_champsim.sh`

# Run simulation

**Single-core simulation**
```
./[BINARY] -warmup_instructions [N_WARM] -simulation_instructions [N_SIM] [TRACE_DIR]/[TRACE]
$ ./ip_stride-asp-1core -warmup_instructions 25000000 -simulation_instructions 25000000 -traces ../traces/trace1.champsimtrace.xz

${BINARY}: ChampSim binary compiled by "build_champsim.sh" (ip_stride-asp-1core)
${N_WARM}: number of instructions for warmup (25 million)
${N_SIM}:  number of instructinos for detailed simulation (25 million)
${TRACE_DIR}: directory where the trace is located (../traces/)
${TRACE}: trace name (trace1.champsimtrace.xz)
```

# Evaluate Simulations

ChampSim measures the IPC (Instruction Per Cycle) value as a performance metric. <br>
There are some other useful metrics printed out at the end of simulation. <be>


## Steps to download gcc version 7 in ubuntu:
1. sudo apt update
2. sudo add-apt-repository ppa:ubuntu-toolchain-r/test
3. vim /etc/apt/sources.list
4. sudo nano /etc/apt/sources.list
5. Update the last line with deb [arch=amd64] http://archive.ubuntu.com/ubuntu focal main universe
6. sudo add-apt-repository ppa:ubuntu-toolchain-r/test
7. sudo apt-get install gcc-7
8. sudo apt-get install g++-7
9. sudo update-alternatives -install /usr/bin/g++ g++ /usr/bin/g++-7 0
10. sudo update-alternatives -install /us/bin/gcc gcc /ust/bin/gcc-7 0
    
--In case the GCC and G++ is already present in /usr/bin (run ./gcc-7 -v in /usr/bin), install the alternative and set it using

1. sudo update-alternatives --config g++
2. sudo update-alternatives --config gcc
