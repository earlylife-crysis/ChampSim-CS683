#!/bin/sh

# DO NOT REMOVE THE " "
CHAMPSIM_DIR="/full-path-to/ChampSim-SC"

PATH_CACHE_H="/full-path-to/ChampSim-SC/inc"

PATH_MAIN_CC="/full-path-to/ChampSim-SC/src"

STLB_PREF='no'
PQ_SIZE='64'
P2TLB='0'
FP_MODE='0'
FP_PREF_MODE='0'
PC_TABLE='128'
PC_TABLE_ASSOC='128'
CLOUDSUITE='0'
STLB_SIZE='256'
STLB_ASSOC='6'
STLB_LAT='8'
BRANCH_PRED="hashed_perceptron"
L1D_PRED="next_line"
L2C_PRED="spp_dev"
PAGE_SIZE='4kb'
MARKOV_SETS='256'
MARKOV_ASSOC='256'
LA_DEPTH='0'
RESET_FREQ='5000'
SUCCESSORS='2'
REPLACEMENT_POLICY='0' # 0:LRU, 1:LFU, 2:RANDOM
SREPLACEMENT_POLICY='0' # 0:FIFO, 1:RANDOM, 2:LRU
LLIMIT='5' 
CNF_BITS='3'
ASAP='0'
IDEAL='0'
OPTIONAL=''

L1I_PRED="next_line"
BPBP_FILTER="0"

PT_S1_SETS="128"
PT_S1_ASSOC="32"

PT_S2_SETS="128"
PT_S2_ASSOC="32"

PT_S4_SETS="128"
PT_S4_ASSOC="32"

PT_S8_SETS="64"
PT_S8_ASSOC="16"

SCS='5000'
MAX_SUCC='7'

# TO AVOID LOOPING FOREVER YOU HAVE TO GIVE AS LAST ARGUMENT --> --end
while true; do
  case "$1" in
    -pref    | --stlb_pref)        		  STLB_PREF=$2;           shift; shift ;;
    -pq      | --pq_size)          		  PQ_SIZE=$2;             shift; shift ;;
    -fp      | --free_prefetching) 		  FP_MODE=$2;             shift; shift ;;
    -fpp     | --free_prefetching_prefetch)       FP_PREF_MODE=$2;        shift; shift ;;
    -pct     | --pc_table)         		  PC_TABLE=$2;            shift; shift ;;
    -pca     | --pc_table_assoc)   		  PC_TABLE_ASSOC=$2;      shift; shift ;;
    -cloud   | --cloudsuite)       		  CLOUDSUITE=$2;          shift; shift ;;  
    -stlbs   | --stlb_size)        		  STLB_SIZE=$2;           shift; shift ;;  
    -stlba   | --stlb_assoc)       		  STLB_ASSOC=$2;          shift; shift ;;  
    -stlblat | --stlb_lat)       		  STLB_LAT=$2;            shift; shift ;;  
    -asap    | --asap)       		          ASAP=$2;                shift; shift ;;  
    -ideal   | --ideal)       		          IDEAL=$2;               shift; shift ;;  
    -p2tlb   | --p2tlb)       		          P2TLB=$2;               shift; shift ;;  
    -pgs     | --page_size)        		  PAGE_SIZE=$2;           shift; shift ;;  
    -mrs     | --markov_sets)      		  MARKOV_SETS=$2;         shift; shift ;;  
    -mra     | --markov_assoc)     		  MARKOV_ASSOC=$2;        shift; shift ;;  
    -lad     | --lookahead_depth)                 LA_DEPTH=$2;            shift; shift ;;  
    -sucs    | --successors)                      SUCCESSORS=$2;          shift; shift ;;  
    -resf    | --reset_freq)                      RESET_FREQ=$2;          shift; shift ;;  
    -rp      | --replacement_policy)              REPLACEMENT_POLICY=$2;  shift; shift ;;  
    -rps     | --successor_rp)                    SREPLACEMENT_POLICY=$2; shift; shift ;;  
    -ll      | --llimit)              	          LLIMIT=$2;              shift; shift ;;  
    -cnf     | --conf_bits)              	  CNF_BITS=$2;            shift; shift ;;  
    -l1ip    | --l1i_pref)        		  L1I_PRED=$2;            shift; shift ;;
    -bpbp    | --bp_bp)        		          BPBP_FILTER=$2;         shift; shift ;;
    -scs     | --succ_size)                       SCS=$2;                 shift; shift ;;
    -msucc   | --max_succ)                        MAX_SUCC=$2;            shift; shift ;;
    -s1s     | --s1_s)                            PT_S1_SETS=$2;          shift; shift ;;
    -s1a     | --s1_a)                            PT_S1_ASSOC=$2;         shift; shift ;;
    -s2s     | --s2_s)                            PT_S2_SETS=$2;          shift; shift ;;
    -s2a     | --s2_a)                            PT_S2_ASSOC=$2;         shift; shift ;;
    -s4s     | --s4_s)                            PT_S4_SETS=$2;          shift; shift ;;
    -s4a     | --s4_a)                            PT_S4_ASSOC=$2;         shift; shift ;;
    -s8s     | --s8_s)                            PT_S8_SETS=$2;          shift; shift ;;
    -s8a     | --s8_a)                            PT_S8_ASSOC=$2;         shift; shift ;;

    -opt   | --optional)         		  OPTIONAL=$2;            shift; shift ;;
    --end) shift; break;;
    * ) shift; continue ;;
  esac
done 

# Page Size (4KB or 2MB)
if [ ${PAGE_SIZE} = "4kb" ]; then
	sed -i '/#define PAGE_SIZE/c\#define PAGE_SIZE 4096' ${PATH_CACHE_H}/champsim.h
	sed -i '/#define LOG2_PAGE_SIZE/c\#define LOG2_PAGE_SIZE 12' ${PATH_CACHE_H}/champsim.h
fi

if [ ${PAGE_SIZE} = "2mb" ]; then
	sed -i '/#define PAGE_SIZE/c\#define PAGE_SIZE 2097152' ${PATH_CACHE_H}/champsim.h
	sed -i '/#define LOG2_PAGE_SIZE/c\#define LOG2_PAGE_SIZE 21' ${PATH_CACHE_H}/champsim.h
fi

# Second Level TLB 
sed -i '/#define STLB_SET/c\#define STLB_SET '${STLB_SIZE}'' ${PATH_CACHE_H}/cache.h
sed -i '/#define STLB_WAY/c\#define STLB_WAY '${STLB_ASSOC}'' ${PATH_CACHE_H}/cache.h
sed -i '/#define STLB_LATENCY/c\#define STLB_LATENCY '${STLB_LAT}'' ${PATH_CACHE_H}/cache.h

# Morrigan
if [ ${STLB_PREF} = "morriganPT" ]; then
    sed -i '/#define PT_S1_SETS/c\#define PT_S1_SETS '${PT_S1_SETS}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S1_ASSOC/c\#define PT_S1_ASSOC '${PT_S1_ASSOC}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S2_SETS/c\#define PT_S2_SETS '${PT_S2_SETS}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S2_ASSOC/c\#define PT_S2_ASSOC '${PT_S2_ASSOC}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S4_SETS/c\#define PT_S4_SETS '${PT_S4_SETS}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S4_ASSOC/c\#define PT_S4_ASSOC '${PT_S4_ASSOC}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S8_SETS/c\#define PT_S8_SETS '${PT_S8_SETS}'' ${PATH_CACHE_H}/morriganPT.h
    sed -i '/#define PT_S8_ASSOC/c\#define PT_S8_ASSOC '${PT_S8_ASSOC}'' ${PATH_CACHE_H}/morriganPT.h
fi

# PREFETCH QUEUE SIZE
sed -i '/#define STLB_PQ_SIZE/c\#define STLB_PQ_SIZE '${PQ_SIZE}'' ${PATH_CACHE_H}/cache.h

# Prefetching directly to TLB 
sed -i '/#define P2TLB/c\#define P2TLB '${P2TLB}'' ${PATH_CACHE_H}/cache.h

# ASAP
sed -i '/#define ASAP/c\#define ASAP '${ASAP}'' ${PATH_CACHE_H}/ooo_cpu.h

# IDEAL
sed -i '/#define IDEAL/c\#define IDEAL '${IDEAL}'' ${PATH_CACHE_H}/ooo_cpu.h

# SOTA TABLE SIZE AND ASSOCIATIVITY 
if [ ${STLB_PREF} = "dp" ] || [ ${STLB_PREF} = "asp" ]; then
	sed -i '/#define TABLE_SIZE/c\#define TABLE_SIZE '${PC_TABLE}'' ${CHAMPSIM_DIR}/prefetcher/${STLB_PREF}.stlb_pref
    sed -i '/#define ASSOC/c\#define ASSOC '${PC_TABLE_ASSOC}'' ${CHAMPSIM_DIR}/prefetcher/${STLB_PREF}.stlb_pref
fi

# SOTA MARKOV TABLE SIZE AND ASSOCIATIVITY 
if [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define MARKOV_TABLE_SETS/c\#define MARKOV_TABLE_SETS '${MARKOV_SETS}'' ${CHAMPSIM_DIR}/prefetcher/${STLB_PREF}.stlb_pref
    sed -i '/#define MARKOV_ASSOC/c\#define MARKOV_ASSOC '${MARKOV_ASSOC}'' ${CHAMPSIM_DIR}/prefetcher/${STLB_PREF}.stlb_pref
fi

# FREE PREFETCHING (DEMAND) 
sed -i '/#define ENABLE_FP/c\#define ENABLE_FP '${FP_MODE}'' ${PATH_CACHE_H}/cache.h

# FREE PREFETCHING (PREFETCH)
sed -i '/#define ENABLE_PREF_FP/c\#define ENABLE_PREF_FP '${FP_PREF_MODE}'' ${PATH_CACHE_H}/cache.h

# LOOKAHEAD DEPTH 
if [ ${STLB_PREF} = "markov_sota" ]; then
	sed -i '/#define LA_DEPTH/c\#define LA_DEPTH '${LA_DEPTH}'' ${PATH_CACHE_H}/cache.h
fi

# SUCCESSORS
if [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define SUCCESSORS/c\#define SUCCESSORS '${SUCCESSORS}'' ${PATH_CACHE_H}/cache.h
fi

# RESET FREQUENCY
if [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define RESET_FREQ/c\#define RESET_FREQ '${RESET_FREQ}'' ${PATH_CACHE_H}/cache.h
fi

# REPLACEMENT POLICY
if [ ${STLB_PREF} = "morriganPT" ] || [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define RP_MP/c\#define RP_MP '${REPLACEMENT_POLICY}'' ${PATH_CACHE_H}/cache.h
fi

# LLIMIT
if [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define LLIMIT/c\#define LLIMIT '${LLIMIT}'' ${PATH_CACHE_H}/cache.h
fi

# BITS OF THE CONFIDENCE COUNTERS FOR THE SUCCESSORS 
if [ ${STLB_PREF} = "morriganPT" ] || [ ${STLB_PREF} = "markov_sota" ]; then
    sed -i '/#define CNF_BITS/c\#define CNF_BITS '${CNF_BITS}'' ${PATH_CACHE_H}/cache.h
fi

# SUCCESSOR REPLACEMENT POLICY
if [ ${STLB_PREF} = "morriganPT" ] || [ ${STLB_PREF} = "maorrigan" ]; then
    sed -i '/#define RP_SUC_MP/c\#define RP_SUC_MP '${SREPLACEMENT_POLICY}'' ${PATH_CACHE_H}/cache.h
fi

# NAME THE BINARY AND THE OUT FOLDER
if [ ${FP_PREF_MODE} = "0" ]; then
	./build_champsim.sh ${BRANCH_PRED} ${L1I_PRED} ${L1D_PRED} ${L2C_PRED} no lru 1 ${STLB_PREF} ${PQ_SIZE}nofp${OPTIONAL}
else
        ./build_champsim.sh ${BRANCH_PRED} ${L1I_PRED} ${L1D_PRED} ${L2C_PRED} no lru 1 ${STLB_PREF} ${PQ_SIZE}fp${OPTIONAL}
fi

echo -e "\n\n"
echo "*****************************************************************************************************************************************"
echo "************************************************************ COMPILE SUCCESS ************************************************************"
echo "*****************************************************************************************************************************************"
echo -e "\n\n"
