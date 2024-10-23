#!/bin/sh

# compile binaries for baseline (no tlb prefetcher) and the prior data tlb prefetchers
#
STLB_PREF='no sp dp asp markov_sota'
for i in ${STLB_PREF}; do
	bash generate_binary.sh --stlb_pref ${i} --end
done


# compile binary of Morrigan
#
STLB_PREF='morriganPT'
FP_PREF_MODE='1' # set it to 1 only for Morrigan
REPLACEMENT_POLICY='1' # rlfu policy 
SUCCESSOR_REPLACEMENT_POLICY='2' # confidence-based replacement of successors
bash generate_binary.sh --stlb_pref ${STLB_PREF} -fpp ${FP_PREF_MODE} --replacement_policy ${REPLACEMENT_POLICY} --successor_rp ${SUCCESSOR_REPLACEMENT_POLICY} --end


# compile binary of ASAP (paper Prefetched Adress Translation)
#
ASAP='1'
bash generate_binary.sh --asap ${ASAP} --optional ASAP --end


# compile binary of Morrigan combined with ASAP
#
STLB_PREF='morriganPT'
FP_PREF_MODE='1' # set it to 1 only for Morrigan
REPLACEMENT_POLICY='1' # rlfu policy 
SUCCESSOR_REPLACEMENT_POLICY='2' # confidence-based replacement of successors
ASAP='1'
bash generate_binary.sh --stlb_pref ${STLB_PREF} -fpp ${FP_PREF_MODE} --replacement_policy ${REPLACEMENT_POLICY} --successor_rp ${SUCCESSOR_REPLACEMENT_POLICY} --asap ${ASAP} --optional ASAP --end


# compile binary of Ideal L2-TLB for instruction accesses
#
STLB_PREF='no'
IDEAL='1'
bash generate_binary.sh --stlb_pref ${STLB_PREF} --ideal ${IDEAL} --optional IDEAL --end


# compile binary of ISO-storage
#
STLB_PREF='no'
STLB_SIZE='276'
STLB_ASSOC='7'
STLB_LAT='10'
bash generate_binary.sh --stlb_pref ${STLB_PREF} --stlb_size ${STLB_SIZE} --stlb_assoc ${STLB_ASSOC} --stlb_lat ${STLB_LAT} --optional ISO --end


# compile binary of prefetching directly into the TLB
#
STLB_PREF='morriganPT'
FP_PREF_MODE='1' # set it to 1 only for Morrigan
REPLACEMENT_POLICY='1' # rlfu policy 
SUCCESSOR_REPLACEMENT_POLICY='2' # confidence-based replacement of successors
P2TLB='1'
bash generate_binary.sh --stlb_pref ${STLB_PREF} -fpp ${FP_PREF_MODE} --replacement_policy ${REPLACEMENT_POLICY} --successor_rp ${SUCCESSOR_REPLACEMENT_POLICY} --p2tlb ${P2TLB} --optional P2TLB --end
