#include "cache.h"

void CACHE::stlb_prefetcher_initialize() 
{

}

void CACHE::stlb_prefetcher_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, int answer, int warmup, int * free_indexes, uint64_t instr_id, int iflag){
}

void CACHE::stlb_prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr)
{

}

void CACHE::stlb_prefetcher_final_stats(uint64_t prefetches, uint64_t hits, uint64_t misses, uint64_t swap, uint64_t dupli, uint64_t free, uint64_t real, uint64_t * mmu_cache_demand_hits, uint64_t * mmu_cache_prefetch_hits, uint64_t * rfhits, uint64_t * free_hits, uint64_t mr[4][4], uint64_t stlb_misses[2])
{
	cout << endl << "*******************************************" << endl;
	cout << "CPU " << cpu << " STLB -- L2-TLB Prefetcher final stats" << endl;
	cout << "*******************************************" << endl;

	cout << endl << "-------------------------------------------" << endl;
	cout << "D-STLB MISSES: " << stlb_misses[0] << endl;
	cout << "I-STLB MISSES: " << stlb_misses[1] << endl;
	cout << "-------------------------------------------" << endl;

	cout << endl << "-------------------------------------------" << endl;
	cout << "PQ hits: " << hits << endl; 
	cout << "PQ misses: " << misses << endl;
	cout << "-------------------------------------------" << endl;
}


