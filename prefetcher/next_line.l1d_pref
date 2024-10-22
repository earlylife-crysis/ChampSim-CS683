#include "cache.h"

// ------------------------- DO NOT CHANGE -------------------------------- //
#define PREFETCH_DEGREE 5             // Prefetch degree
// ------------------------- DO NOT CHANGE -------------------------------- //

// ------------------------- Initialize the prefetcher ------------------------- // 
void CACHE::l1d_prefetcher_initialize() 
{
	cout << "CPU " << cpu << " L1D Next-line prefetcher" << endl;

}

// --------------- This is the main prefetcher operate function ---------------- // 
void CACHE::l1d_prefetcher_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint8_t critical_ip_flag)
{   
    for (int i=0; i<PREFETCH_DEGREE; i++) {
        uint64_t cl_addr = addr >> LOG2_BLOCK_SIZE;

        // ----------------------- Next-line logic ------------------------ // 
        uint64_t pf_address = (cl_addr + ((i+1))) << LOG2_BLOCK_SIZE;

        // only issue a prefetch if the prefetch address is in the same 4 KB page
        // as the current demand access address
        if ((pf_address >> LOG2_PAGE_SIZE) != (addr >> LOG2_PAGE_SIZE))
            break;
            
        prefetch_line(ip, addr, pf_address, FILL_L1, 0);
    }
    return;
}

// ------------------------- DO NOT CHANGE -------------------------------- //
void CACHE::l1d_prefetcher_cache_fill(uint64_t v_addr, uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t v_evicted_addr, uint64_t evicted_addr, uint32_t metadata_in)
{
	return;
}

void CACHE::l1d_prefetcher_final_stats()
{
	cout << "CPU " << cpu << " L1D nextline prefetcher final stats" << endl;
	cout << "Degree: " << PREFETCH_DEGREE << endl;
}
// ------------------------- DO NOT CHANGE -------------------------------- //