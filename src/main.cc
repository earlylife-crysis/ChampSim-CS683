#define _BSD_SOURCE

#include <getopt.h>
#include "ooo_cpu.h"
#include "uncore.h"
#include <fstream>

uint8_t warmup_complete[NUM_CPUS], 
	simulation_complete[NUM_CPUS], 
	all_warmup_complete = 0, 
	all_simulation_complete = 0,
	MAX_INSTR_DESTINATIONS = NUM_INSTR_DESTINATIONS,
	knob_cloudsuite = 0,
	knob_low_bandwidth = 0;

uint64_t warmup_instructions     = 1000000,
	 simulation_instructions = 10000000,
	 champsim_seed;

time_t start_time;

// PAGE TABLE
uint32_t PAGE_TABLE_LATENCY = 0, SWAP_LATENCY = 0;
queue <uint64_t > page_queue;
map <uint64_t, uint64_t> page_table, inverse_table, recent_page, unique_cl[NUM_CPUS];
uint64_t previous_ppage, num_adjacent_page, num_cl[NUM_CPUS], allocated_pages, num_page[NUM_CPUS], minor_fault[NUM_CPUS], major_fault[NUM_CPUS];

void record_roi_stats(uint32_t cpu, CACHE *cache)
{
	for (uint32_t i=0; i<NUM_TYPES; i++) {
		cache->roi_access[cpu][i] = cache->sim_access[cpu][i];
		cache->roi_hit[cpu][i] = cache->sim_hit[cpu][i];
		cache->roi_miss[cpu][i] = cache->sim_miss[cpu][i];
	}
}

void print_roi_stats(uint32_t cpu, CACHE *cache)
{
	uint64_t TOTAL_ACCESS = 0, TOTAL_HIT = 0, TOTAL_MISS = 0;

	for (uint32_t i=0; i<NUM_TYPES; i++) {
		TOTAL_ACCESS += cache->roi_access[cpu][i];
		TOTAL_HIT += cache->roi_hit[cpu][i];
		TOTAL_MISS += cache->roi_miss[cpu][i];
	}

	cout << cache->NAME;
	cout << " TOTAL     ACCESS: " << setw(10) << TOTAL_ACCESS << "  HIT: " << setw(10) << TOTAL_HIT << "  MISS: " << setw(10) << TOTAL_MISS << endl;

	cout << cache->NAME;
	cout << " LOAD      ACCESS: " << setw(10) << cache->roi_access[cpu][0] << "  HIT: " << setw(10) << cache->roi_hit[cpu][0] << "  MISS: " << setw(10) << cache->roi_miss[cpu][0] << endl;

	cout << cache->NAME;
	cout << " RFO       ACCESS: " << setw(10) << cache->roi_access[cpu][1] << "  HIT: " << setw(10) << cache->roi_hit[cpu][1] << "  MISS: " << setw(10) << cache->roi_miss[cpu][1] << endl;

	cout << cache->NAME;
	cout << " PREFETCH  ACCESS: " << setw(10) << cache->roi_access[cpu][2] << "  HIT: " << setw(10) << cache->roi_hit[cpu][2] << "  MISS: " << setw(10) << cache->roi_miss[cpu][2] << endl;

	cout << cache->NAME;
	cout << " WRITEBACK ACCESS: " << setw(10) << cache->roi_access[cpu][3] << "  HIT: " << setw(10) << cache->roi_hit[cpu][3] << "  MISS: " << setw(10) << cache->roi_miss[cpu][3] << endl;

	cout << cache->NAME;
	cout << " PREFETCH  REQUESTED: " << setw(10) << cache->pf_requested << "  ISSUED: " << setw(10) << cache->pf_issued;
	cout << "  USEFUL: " << setw(10) << cache->pf_useful << "  USELESS: " << setw(10) << cache->pf_useless << endl;

	cout << cache->NAME;
	cout << " AVERAGE MISS LATENCY: " << (1.0*(cache->total_miss_latency))/TOTAL_MISS << " cycles" << endl;
	//cout << " AVERAGE MISS LATENCY: " << (cache->total_miss_latency)/TOTAL_MISS << " cycles " << cache->total_miss_latency << "/" << TOTAL_MISS<< endl;
}

void print_sim_stats(uint32_t cpu, CACHE *cache)
{
	uint64_t TOTAL_ACCESS = 0, TOTAL_HIT = 0, TOTAL_MISS = 0;

	for (uint32_t i=0; i<NUM_TYPES; i++) {
		TOTAL_ACCESS += cache->sim_access[cpu][i];
		TOTAL_HIT += cache->sim_hit[cpu][i];
		TOTAL_MISS += cache->sim_miss[cpu][i];
	}

	cout << cache->NAME;
	cout << " TOTAL     ACCESS: " << setw(10) << TOTAL_ACCESS << "  HIT: " << setw(10) << TOTAL_HIT << "  MISS: " << setw(10) << TOTAL_MISS << endl;

	cout << cache->NAME;
	cout << " LOAD      ACCESS: " << setw(10) << cache->sim_access[cpu][0] << "  HIT: " << setw(10) << cache->sim_hit[cpu][0] << "  MISS: " << setw(10) << cache->sim_miss[cpu][0] << endl;

	cout << cache->NAME;
	cout << " RFO       ACCESS: " << setw(10) << cache->sim_access[cpu][1] << "  HIT: " << setw(10) << cache->sim_hit[cpu][1] << "  MISS: " << setw(10) << cache->sim_miss[cpu][1] << endl;

	cout << cache->NAME;
	cout << " PREFETCH  ACCESS: " << setw(10) << cache->sim_access[cpu][2] << "  HIT: " << setw(10) << cache->sim_hit[cpu][2] << "  MISS: " << setw(10) << cache->sim_miss[cpu][2] << endl;

	cout << cache->NAME;
	cout << " WRITEBACK ACCESS: " << setw(10) << cache->sim_access[cpu][3] << "  HIT: " << setw(10) << cache->sim_hit[cpu][3] << "  MISS: " << setw(10) << cache->sim_miss[cpu][3] << endl;
}

void print_branch_stats()
{
	for (uint32_t i=0; i<NUM_CPUS; i++) {
		cout << endl << "CPU " << i << " Branch Prediction Accuracy: ";
		cout << (100.0*(ooo_cpu[i].num_branch - ooo_cpu[i].branch_mispredictions)) / ooo_cpu[i].num_branch;
		cout << "% MPKI: " << (1000.0*ooo_cpu[i].branch_mispredictions)/(ooo_cpu[i].num_retired - ooo_cpu[i].warmup_instructions);
		cout << " Average ROB Occupancy at Mispredict: " << (1.0*ooo_cpu[i].total_rob_occupancy_at_branch_mispredict)/ooo_cpu[i].branch_mispredictions << endl << endl;

		cout << "Branch types" << endl;
		cout << "NOT_BRANCH: " << ooo_cpu[i].total_branch_types[0] << " " << (100.0*ooo_cpu[i].total_branch_types[0])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_DIRECT_JUMP: " << ooo_cpu[i].total_branch_types[1] << " " << (100.0*ooo_cpu[i].total_branch_types[1])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_INDIRECT: " << ooo_cpu[i].total_branch_types[2] << " " << (100.0*ooo_cpu[i].total_branch_types[2])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_CONDITIONAL: " << ooo_cpu[i].total_branch_types[3] << " " << (100.0*ooo_cpu[i].total_branch_types[3])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_DIRECT_CALL: " << ooo_cpu[i].total_branch_types[4] << " " << (100.0*ooo_cpu[i].total_branch_types[4])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_INDIRECT_CALL: " << ooo_cpu[i].total_branch_types[5] << " " << (100.0*ooo_cpu[i].total_branch_types[5])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_RETURN: " << ooo_cpu[i].total_branch_types[6] << " " << (100.0*ooo_cpu[i].total_branch_types[6])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl;
		cout << "BRANCH_OTHER: " << ooo_cpu[i].total_branch_types[7] << " " << (100.0*ooo_cpu[i].total_branch_types[7])/(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) << "%" << endl << endl;
	}
}

void print_dram_stats()
{
	cout << endl;
	cout << "DRAM Statistics" << endl;
	for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
		cout << " CHANNEL " << i << endl;
		cout << " RQ ROW_BUFFER_HIT: " << setw(10) << uncore.DRAM.RQ[i].ROW_BUFFER_HIT << "  ROW_BUFFER_MISS: " << setw(10) << uncore.DRAM.RQ[i].ROW_BUFFER_MISS << endl;
		cout << " DBUS_CONGESTED: " << setw(10) << uncore.DRAM.dbus_congested[NUM_TYPES][NUM_TYPES] << endl; 
		cout << " WQ ROW_BUFFER_HIT: " << setw(10) << uncore.DRAM.WQ[i].ROW_BUFFER_HIT << "  ROW_BUFFER_MISS: " << setw(10) << uncore.DRAM.WQ[i].ROW_BUFFER_MISS;
		cout << "  FULL: " << setw(10) << uncore.DRAM.WQ[i].FULL << endl; 
		cout << endl;
	}

	uint64_t total_congested_cycle = 0;
	for (uint32_t i=0; i<DRAM_CHANNELS; i++)
		total_congested_cycle += uncore.DRAM.dbus_cycle_congested[i];
	if (uncore.DRAM.dbus_congested[NUM_TYPES][NUM_TYPES])
		cout << " AVG_CONGESTED_CYCLE: " << (total_congested_cycle / uncore.DRAM.dbus_congested[NUM_TYPES][NUM_TYPES]) << endl;
	else
		cout << " AVG_CONGESTED_CYCLE: -" << endl;
}

void reset_cache_stats(uint32_t cpu, CACHE *cache)
{
	for (uint32_t i=0; i<NUM_TYPES; i++) {
		cache->ACCESS[i] = 0;
		cache->HIT[i] = 0;
		cache->MISS[i] = 0;
		cache->MSHR_MERGED[i] = 0;
		cache->STALL[i] = 0;

		cache->sim_access[cpu][i] = 0;
		cache->sim_hit[cpu][i] = 0;
		cache->sim_miss[cpu][i] = 0;
	}

	cache->total_miss_latency = 0;

	cache->RQ.ACCESS = 0;
	cache->RQ.MERGED = 0;
	cache->RQ.TO_CACHE = 0;

	cache->WQ.ACCESS = 0;
	cache->WQ.MERGED = 0;
	cache->WQ.TO_CACHE = 0;
	cache->WQ.FORWARD = 0;
	cache->WQ.FULL = 0;


	if(cache->NAME == "STLB"){
		cache->pf_total_pq = 0;
		cache->pf_hits_pq  = 0;
		cache->pf_misses_pq = 0;
		cache->pf_swap = 0;
		cache->pf_dupli = 0;
		cache->pf_free = 0;
		cache->pf_real = 0;
		cache->fctb_hits = 0;
		cache->fctb_misses = 0;
		cache->issued_prefetches_lad = 0;
		cache->hit_prefetches_lad = 0;
		for(int mmu=0; mmu<4; mmu++){
			cache->mmu_cache_demand_hits[mmu] = 0;
			cache->mmu_cache_prefetch_hits[mmu] = 0;
		}
		for(int mmu=0; mmu<14; mmu++){
			cache->free_hits[mmu] = 0;
		}
		cache->rfhits[0] = 0;
		cache->rfhits[1] = 0;

		for(int ii=0; ii<4; ii++){
			for(int jj=0; jj<4; jj++){
				cache->pagetable_mr_hit_ratio[ii][jj] = 0;
			}
		}

		cache->stlb_misses[0] = 0;
		cache->stlb_misses[1] = 0;

		cache->bpbp[0] = 0;
		cache->bpbp[1] = 0;
		cache->bpbp[2] = 0;
		cache->bpbp[3] = 0;
		cache->bpbp[4] = 0;

		cache->morrigan_filter_hits = 0;
		//cache->vtb_hits = 0;
		//cache->vtb_misses = 0;
	}

}

void finish_warmup()
{
	uint64_t elapsed_second = (uint64_t)(time(NULL) - start_time),
		 elapsed_minute = elapsed_second / 60,
		 elapsed_hour = elapsed_minute / 60;
	elapsed_minute -= elapsed_hour*60;
	elapsed_second -= (elapsed_hour*3600 + elapsed_minute*60);

	// reset core latency
	// note: since re-ordering he function calls in the main simulation loop, it's no longer necessary to add
	//       extra latency for scheduling and execution, unless you want these steps to take longer than 1 cycle.
	SCHEDULING_LATENCY = 0;
	EXEC_LATENCY = 0;
	DECODE_LATENCY = 2;
	PAGE_TABLE_LATENCY = 100;
	SWAP_LATENCY = 100000;

	cout << endl;
	for (uint32_t i=0; i<NUM_CPUS; i++) {
		cout << "Warmup complete CPU " << i << " instructions: " << ooo_cpu[i].num_retired << " cycles: " << current_core_cycle[i];
		cout << " (Simulation time: " << elapsed_hour << " hr " << elapsed_minute << " min " << elapsed_second << " sec) " << endl;

		ooo_cpu[i].begin_sim_cycle = current_core_cycle[i]; 
		ooo_cpu[i].begin_sim_instr = ooo_cpu[i].num_retired;

		// reset branch stats
		ooo_cpu[i].num_branch = 0;
		ooo_cpu[i].branch_mispredictions = 0;
		ooo_cpu[i].total_rob_occupancy_at_branch_mispredict = 0;

		for(uint32_t j=0; j<8; j++)
		{
			ooo_cpu[i].total_branch_types[j] = 0;
		}

		reset_cache_stats(i, &ooo_cpu[i].L1I);
		reset_cache_stats(i, &ooo_cpu[i].L1D);
		reset_cache_stats(i, &ooo_cpu[i].L2C);
		reset_cache_stats(i, &ooo_cpu[i].STLB);
		reset_cache_stats(i, &ooo_cpu[i].DTLB);
		reset_cache_stats(i, &ooo_cpu[i].ITLB);
		reset_cache_stats(i, &uncore.LLC);
	}
	cout << endl;

	// reset DRAM stats
	for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
		uncore.DRAM.RQ[i].ROW_BUFFER_HIT = 0;
		uncore.DRAM.RQ[i].ROW_BUFFER_MISS = 0;
		uncore.DRAM.WQ[i].ROW_BUFFER_HIT = 0;
		uncore.DRAM.WQ[i].ROW_BUFFER_MISS = 0;
	}

	// set actual cache latency
	for (uint32_t i=0; i<NUM_CPUS; i++) {
		ooo_cpu[i].ITLB.LATENCY = ITLB_LATENCY;
		ooo_cpu[i].DTLB.LATENCY = DTLB_LATENCY;
		ooo_cpu[i].STLB.LATENCY = STLB_LATENCY;
		ooo_cpu[i].L1I.LATENCY  = L1I_LATENCY;
		ooo_cpu[i].L1D.LATENCY  = L1D_LATENCY;
		ooo_cpu[i].L2C.LATENCY  = L2C_LATENCY;
	}
	uncore.LLC.LATENCY = LLC_LATENCY;
}

void print_deadlock(uint32_t i)
{
	cout << "DEADLOCK! CPU " << i << " instr_id: " << ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].instr_id;
	cout << " translated: " << +ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].translated;
	cout << " fetched: " << +ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].fetched;
	cout << " scheduled: " << +ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].scheduled;
	cout << " executed: " << +ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].executed;
	cout << " is_memory: " << +ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].is_memory;
	cout << " event: " << ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].event_cycle;
	cout << " current: " << current_core_cycle[i] << endl;

	// print LQ entry
	cout << endl << "Load Queue Entry" << endl;
	for (uint32_t j=0; j<LQ_SIZE; j++) {
		cout << "[LQ] entry: " << j << " instr_id: " << ooo_cpu[i].LQ.entry[j].instr_id << " address: " << hex << ooo_cpu[i].LQ.entry[j].physical_address << dec << " translated: " << +ooo_cpu[i].LQ.entry[j].translated << " fetched: " << +ooo_cpu[i].LQ.entry[i].fetched << endl;
	}

	// print SQ entry
	cout << endl << "Store Queue Entry" << endl;
	for (uint32_t j=0; j<SQ_SIZE; j++) {
		cout << "[SQ] entry: " << j << " instr_id: " << ooo_cpu[i].SQ.entry[j].instr_id << " address: " << hex << ooo_cpu[i].SQ.entry[j].physical_address << dec << " translated: " << +ooo_cpu[i].SQ.entry[j].translated << " fetched: " << +ooo_cpu[i].SQ.entry[i].fetched << endl;
	}

	// print L1D MSHR entry
	PACKET_QUEUE *queue;
	queue = &ooo_cpu[i].L1D.MSHR;
	cout << endl << queue->NAME << " Entry" << endl;
	for (uint32_t j=0; j<queue->SIZE; j++) {
		cout << "[" << queue->NAME << "] entry: " << j << " instr_id: " << queue->entry[j].instr_id << " rob_index: " << queue->entry[j].rob_index;
		cout << " address: " << hex << queue->entry[j].address << " full_addr: " << queue->entry[j].full_addr << dec << " type: " << +queue->entry[j].type;
		cout << " fill_level: " << queue->entry[j].fill_level << " lq_index: " << queue->entry[j].lq_index << " sq_index: " << queue->entry[j].sq_index << endl; 
	}

	assert(0);
}

void signal_handler(int signal) 
{
	cout << "Caught signal: " << signal << endl;
	exit(1);
}

// log base 2 function from efectiu
int lg2(int n)
{
	int i, m = n, c = -1;
	for (i=0; m; i++) {
		m /= 2;
		c++;
	}
	return c;
}

uint64_t rotl64 (uint64_t n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT*sizeof(n)-1);

	assert ( (c<=mask) &&"rotate by type width or more");
	c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
	return (n<<c) | (n>>( (-c)&mask ));
}

uint64_t rotr64 (uint64_t n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT*sizeof(n)-1);

	assert ( (c<=mask) &&"rotate by type width or more");
	c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
	return (n>>c) | (n<<( (-c)&mask ));
}

RANDOM champsim_rand(champsim_seed);


pair<uint64_t,uint64_t> va_to_pa(uint32_t cpu, uint64_t instr_id, uint64_t va, uint64_t unique_vpage, uint64_t ip, int type, int iflag, bool magic)
{
#ifdef SANITY_CHECK
	if (va == 0) 
		assert(0);
#endif
	uint64_t cstall = 2;
	uint8_t  swap = 0;
	uint64_t high_bit_mask = rotr64(cpu, lg2(NUM_CPUS)),
		 unique_va = va | high_bit_mask;
	//uint64_t vpage = unique_va >> LOG2_PAGE_SIZE,
	uint64_t vpage = unique_vpage | high_bit_mask,
		 voffset = unique_va & ((1<<LOG2_PAGE_SIZE) - 1);

	// smart random number generator
	uint64_t random_ppage;

	map <uint64_t, uint64_t>::iterator pr = page_table.begin();
	map <uint64_t, uint64_t>::iterator ppage_check = inverse_table.begin();

	// check unique cache line footprint
	map <uint64_t, uint64_t>::iterator cl_check = unique_cl[cpu].find(unique_va >> LOG2_BLOCK_SIZE);
	if (cl_check == unique_cl[cpu].end()) { // we've never seen this cache line before
		unique_cl[cpu].insert(make_pair(unique_va >> LOG2_BLOCK_SIZE, 0));
		num_cl[cpu]++;
	}
	else
		cl_check->second++;

	pr = page_table.find(vpage);
	if (pr == page_table.end()) { // no VA => PA translation found 

		if (allocated_pages >= DRAM_PAGES) { // not enough memory

			uint8_t  found_NRU = 0;
			uint64_t NRU_vpage = 0; // implement it
			map <uint64_t, uint64_t>::iterator pr2 = recent_page.begin();
			for (pr = page_table.begin(); pr != page_table.end(); pr++) {

				NRU_vpage = pr->first;
				if (recent_page.find(NRU_vpage) == recent_page.end()) {
					found_NRU = 1;
					break;
				}
			}
#ifdef SANITY_CHECK
			if (found_NRU == 0)
				assert(0);

			if (pr == page_table.end())
				assert(0);
#endif
			DP ( if (warmup_complete[cpu]) {
					cout << "[SWAP] update page table NRU_vpage: " << hex << pr->first << " new_vpage: " << vpage << " ppage: " << pr->second << dec << endl; });

			uint64_t mapped_ppage = pr->second;
			page_table.erase(pr);
			page_table.insert(make_pair(vpage, mapped_ppage));

			ppage_check = inverse_table.find(mapped_ppage);
#ifdef SANITY_CHECK
			if (ppage_check == inverse_table.end())
				assert(0);
#endif
			ppage_check->second = vpage;

			DP ( if (warmup_complete[cpu]) {
					cout << "[SWAP] update inverse table NRU_vpage: " << hex << NRU_vpage << " new_vpage: ";
					cout << ppage_check->second << " ppage: " << ppage_check->first << dec << endl; });

			page_queue.pop();
			page_queue.push(vpage);

			ooo_cpu[cpu].ITLB.invalidate_entry(NRU_vpage);
			ooo_cpu[cpu].DTLB.invalidate_entry(NRU_vpage);
			ooo_cpu[cpu].STLB.invalidate_entry(NRU_vpage);
			for (uint32_t i=0; i<BLOCK_SIZE; i++) {
				uint64_t cl_addr = (mapped_ppage << 6) | i;
				ooo_cpu[cpu].L1I.invalidate_entry(cl_addr);
				ooo_cpu[cpu].L1D.invalidate_entry(cl_addr);
				ooo_cpu[cpu].L2C.invalidate_entry(cl_addr);
				uncore.LLC.invalidate_entry(cl_addr);
			}

			swap = 1;
		} else {
			uint8_t fragmented = 0;
			if (num_adjacent_page > 0)
				random_ppage = ++previous_ppage;
			else {
				random_ppage = champsim_rand.draw_rand();
				fragmented = 1;
			}

			while (1) { 
				ppage_check = inverse_table.find(random_ppage); 
				if (ppage_check != inverse_table.end()) { 
					DP ( if (warmup_complete[cpu]) {
							cout << "vpage: " << hex << ppage_check->first << " is already mapped to ppage: " << random_ppage << dec << endl; }); 

					if (num_adjacent_page > 0)
						fragmented = 1;

					random_ppage = champsim_rand.draw_rand();
				}
				else
					break;
			}

			page_table.insert(make_pair(vpage, random_ppage));
			inverse_table.insert(make_pair(random_ppage, vpage));
			page_queue.push(vpage);
			previous_ppage = random_ppage;
			num_adjacent_page--;
			num_page[cpu]++;
			allocated_pages++;

			if (fragmented) {
				num_adjacent_page = 1 << (rand() % 10);
				DP ( if (warmup_complete[cpu]) {
						cout << "Recalculate num_adjacent_page: " << num_adjacent_page << endl; });
			}
		}

		if (swap)
			major_fault[cpu]++;
		else
			minor_fault[cpu]++;
	}
	else {
		//printf("Found  vpage: %lx  random_ppage: %lx\n", vpage, pr->second);
	}

	pr = page_table.find(vpage);
#ifdef SANITY_CHECK
	if (pr == page_table.end())
		assert(0);
#endif
	uint64_t ppage = pr->second;

	uint64_t pa = ppage << LOG2_PAGE_SIZE;
	pa |= voffset;

	DP ( if (warmup_complete[cpu]) {
			cout << "[PAGE_TABLE] instr_id: " << instr_id << " vpage: " << hex << vpage;
			cout << " => ppage: " << (pa >> LOG2_PAGE_SIZE) << " vadress: " << unique_va << " paddress: " << pa << dec << endl; });

	int mmu_hit[3] = {0,0,0};
	uint64_t compare_pml4, compare_pdp, compare_pd;

	if(LOG2_PAGE_SIZE == 12){
		compare_pml4 = vpage>>27; 
		compare_pdp  = vpage>>18; 
		compare_pd   = vpage>>9; 
	}
	else{
		compare_pml4 = vpage>>18; 
		compare_pdp  = vpage>>9; 
	}

	ooo_cpu[cpu].STLB.mmu_timer++;

	mmu_hit[0] = ooo_cpu[cpu].STLB.search_pml4(compare_pml4);
	mmu_hit[1] = ooo_cpu[cpu].STLB.search_pdp(compare_pdp);

	if(LOG2_PAGE_SIZE == 12)
		mmu_hit[2] = ooo_cpu[cpu].STLB.search_pd(compare_pd);

	if (mmu_hit[2] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_demand_hits[2]++;
	else if (mmu_hit[1] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_demand_hits[1]++;
	else if (mmu_hit[0] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_demand_hits[0]++;
	else if (iflag)
		ooo_cpu[cpu].STLB.mmu_cache_demand_hits[3]++;
	else 
		int hw = 0;

	ooo_cpu[cpu].STLB.lru_pml4(ooo_cpu[cpu].STLB.mmu_timer, compare_pml4);

	ooo_cpu[cpu].STLB.lru_pdp(ooo_cpu[cpu].STLB.mmu_timer, compare_pdp);

	if(LOG2_PAGE_SIZE == 12)
		ooo_cpu[cpu].STLB.lru_pd(ooo_cpu[cpu].STLB.mmu_timer, compare_pd);

	if (swap){
		if(magic != 2)
			stall_cycle[cpu] = current_core_cycle[cpu] + SWAP_LATENCY;
	}
	else{
		uint64_t cr3 = 0x200000;

		uint64_t pt_index, pd_index, pdp_index, pml4_index;

		if(LOG2_PAGE_SIZE == 12){
			pt_index   = (vpage & 0x00000000001ff);
			pd_index   = ((vpage>>9) & 0x00000000001ff);
			pdp_index  = ((vpage>>18) & 0x00000000001ff);
			pml4_index = ((vpage>>27) & 0x00000000001ff);
		}
		else{
			pt_index   = (vpage & 0x00000000001ff);
			pdp_index  = ((vpage>>9) & 0x00000000001ff);
			pml4_index = ((vpage>>18) & 0x00000000001ff);
		}

		uint64_t pml42s, pdp2s, pd2s, pt2s;

		if(LOG2_PAGE_SIZE == 12){
			pml42s = cr3 + pml4_index * 8;

			pdp2s  = cr3 + 512 * 8 \
				 + pml4_index * 512 * 8 \
				 + pdp_index * 8;

			pd2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 8 \
			       + pdp_index * 512 * 8 \
			       + pd_index * 8;

			pt2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + 512 * 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 512 * 8 \
			       + pdp_index * 512 * 512 * 8 \
			       + pd_index * 512 * 8\
			       + pt_index * 8;
		}
		else{
			pml42s = cr3 + pml4_index * 8;

			pdp2s  = cr3 + 512 * 8 \
				 + pml4_index * 512 * 8 \
				 + pdp_index * 8;

			pt2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 8 \
			       + pdp_index * 512 * 8 \
			       + pt_index * 8;
		}

		cstall = 2;

		uint32_t set, way_fill;
		int way_read;

		int debug = 0; 

		if(debug) cout << "\nNEW PAGE WALK " << endl;

		int asap = ASAP;
		int ideal = iflag * IDEAL;

		if(!asap && ideal!=1){
			if(mmu_hit[0] == 0){
				PACKET search_packet;
				search_packet.address = pml42s;

				set = ooo_cpu[cpu].L1D.get_set(pml42s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);

				if(way_read >=0){
					if(!magic){
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L1D.block[set][way_read].full_addr, ip, 0, type, 1);
						ooo_cpu[cpu].L1D.block[set][way_read].used = 1;
					}
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;				

					if(!magic){
						way_fill = ooo_cpu[cpu].L1D.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L1D.block[set], ip, pml42s, type);
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_fill, pml42s, ip, 0, type, 0);

						if (ooo_cpu[cpu].L1D.block[set][way_fill].valid == 0)
							ooo_cpu[cpu].L1D.block[set][way_fill].valid = 1;

						ooo_cpu[cpu].L1D.block[set][way_fill].dirty = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].prefetch = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].used = 0;

						ooo_cpu[cpu].L1D.block[set][way_fill].tag = pml42s;
						ooo_cpu[cpu].L1D.block[set][way_fill].address = pml42s;
						ooo_cpu[cpu].L1D.block[set][way_fill].full_addr = pml42s;
						ooo_cpu[cpu].L1D.block[set][way_fill].data = 55;
						ooo_cpu[cpu].L1D.block[set][way_fill].cpu = cpu;
					}

					set = ooo_cpu[cpu].L2C.get_set(pml42s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);

					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][1]++;
						if(!magic){
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L2C.block[set][way_read].full_addr, ip, 0, type, 1);
							ooo_cpu[cpu].L2C.block[set][way_read].used = 1;
						}
						cstall+=L2C_LATENCY;
					}
					else{
						cstall+=L2C_LATENCY;

						if(!magic){
							way_fill = ooo_cpu[cpu].L2C.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L2C.block[set], ip, pml42s, type);
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_fill, pml42s, ip, 0, type, 0);

							if (ooo_cpu[cpu].L2C.block[set][way_fill].valid == 0)
								ooo_cpu[cpu].L2C.block[set][way_fill].valid = 1; 

							ooo_cpu[cpu].L2C.block[set][way_fill].dirty = 0; 
							ooo_cpu[cpu].L2C.block[set][way_fill].prefetch = 0; 
							ooo_cpu[cpu].L2C.block[set][way_fill].used = 0; 

							ooo_cpu[cpu].L2C.block[set][way_fill].tag = pml42s;
							ooo_cpu[cpu].L2C.block[set][way_fill].address = pml42s;
							ooo_cpu[cpu].L2C.block[set][way_fill].full_addr = pml42s;
							ooo_cpu[cpu].L2C.block[set][way_fill].data = 55;
							ooo_cpu[cpu].L2C.block[set][way_fill].cpu = cpu; 
						}

						set = uncore.LLC.get_set(pml42s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][2]++;
							if(!magic){
								uncore.LLC.llc_update_replacement_state(cpu, set, way_read, uncore.LLC.block[set][way_read].full_addr, ip, 0, type, 1);
								uncore.LLC.block[set][way_read].used = 1;
							}
							cstall+=LLC_LATENCY;
						}
						else{
							cstall+=LLC_LATENCY;

							if(!magic){
								way_fill = uncore.LLC.llc_find_victim(cpu, instr_id, set, uncore.LLC.block[set], ip, pml42s, type);
								uncore.LLC.llc_update_replacement_state(cpu, set, way_fill, pml42s, ip, 0, type, 0);

								if (uncore.LLC.block[set][way_fill].valid == 0)
									uncore.LLC.block[set][way_fill].valid = 1;

								uncore.LLC.block[set][way_fill].dirty = 0;
								uncore.LLC.block[set][way_fill].prefetch = 0;
								uncore.LLC.block[set][way_fill].used = 0;

								uncore.LLC.block[set][way_fill].tag = pml42s;
								uncore.LLC.block[set][way_fill].address = pml42s;
								uncore.LLC.block[set][way_fill].full_addr = pml42s;
								uncore.LLC.block[set][way_fill].data = 55;
								uncore.LLC.block[set][way_fill].cpu = cpu;
							}

							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][3]++;
							cstall += 200;
						}
					}
				}
			}

			if(mmu_hit[1] == 0){
				PACKET search_packet;
				search_packet.address = pdp2s;

				set = ooo_cpu[cpu].L1D.get_set(pdp2s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);

				if(way_read >=0){
					if(!magic){
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L1D.block[set][way_read].full_addr, ip, type, type, 1);
						ooo_cpu[cpu].L1D.block[set][way_read].used = 1;
					}
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;

					if(!magic){
						way_fill = ooo_cpu[cpu].L1D.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L1D.block[set], ip, pdp2s, type);
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_fill, pdp2s, ip, ooo_cpu[cpu].L1D.block[set][way_fill].full_addr, type, 0);

						if (ooo_cpu[cpu].L1D.block[set][way_fill].valid == 0)
							ooo_cpu[cpu].L1D.block[set][way_fill].valid = 1;

						ooo_cpu[cpu].L1D.block[set][way_fill].dirty = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].prefetch = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].used = 0;

						ooo_cpu[cpu].L1D.block[set][way_fill].tag = pdp2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].address = pdp2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].full_addr = pdp2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].data = 55;
						ooo_cpu[cpu].L1D.block[set][way_fill].cpu = cpu;
					}

					set = ooo_cpu[cpu].L2C.get_set(pdp2s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][1]++;
						if(!magic){
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L2C.block[set][way_read].full_addr, ip, type, type, 1);
							ooo_cpu[cpu].L2C.block[set][way_read].used = 1;
						}
						cstall+=L2C_LATENCY;
					}
					else{
						cstall += L2C_LATENCY;

						if(!magic){
							way_fill = ooo_cpu[cpu].L2C.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L2C.block[set], ip, pdp2s, type);
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_fill, pdp2s, ip, ooo_cpu[cpu].L2C.block[set][way_fill].full_addr, type, 0);

							if (ooo_cpu[cpu].L2C.block[set][way_fill].valid == 0)
								ooo_cpu[cpu].L2C.block[set][way_fill].valid = 1;

							ooo_cpu[cpu].L2C.block[set][way_fill].dirty = 0;
							ooo_cpu[cpu].L2C.block[set][way_fill].prefetch = 0;
							ooo_cpu[cpu].L2C.block[set][way_fill].used = 0;

							ooo_cpu[cpu].L2C.block[set][way_fill].tag = pdp2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].address = pdp2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].full_addr = pdp2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].data = 55;
							ooo_cpu[cpu].L2C.block[set][way_fill].cpu = cpu;
						}

						set = uncore.LLC.get_set(pdp2s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][2]++;
							if(!magic){
								uncore.LLC.llc_update_replacement_state(cpu, set, way_read, uncore.LLC.block[set][way_read].full_addr, ip, 0, type, 1);
								uncore.LLC.block[set][way_read].used = 1;
							}
							cstall += LLC_LATENCY;
						}
						else{
							cstall += LLC_LATENCY;

							if(!magic){
								way_fill = uncore.LLC.llc_find_victim(cpu, instr_id, set, uncore.LLC.block[set], ip, pdp2s, type);
								uncore.LLC.llc_update_replacement_state(cpu, set, way_fill, pdp2s, ip, uncore.LLC.block[set][way_fill].full_addr, type, 0);

								if (uncore.LLC.block[set][way_fill].valid == 0)
									uncore.LLC.block[set][way_fill].valid = 1;

								uncore.LLC.block[set][way_fill].dirty = 0;
								uncore.LLC.block[set][way_fill].prefetch = 0;
								uncore.LLC.block[set][way_fill].used = 0;

								uncore.LLC.block[set][way_fill].tag = pdp2s;
								uncore.LLC.block[set][way_fill].address = pdp2s;
								uncore.LLC.block[set][way_fill].full_addr = pdp2s;
								uncore.LLC.block[set][way_fill].data = 55;
								uncore.LLC.block[set][way_fill].cpu = cpu;
							}
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][3]++;
							cstall += 200;
						}
					}
				}
			}

			if((mmu_hit[2] == 0) && (LOG2_PAGE_SIZE == 12)){

				PACKET search_packet;
				search_packet.address = pd2s;

				set = ooo_cpu[cpu].L1D.get_set(pd2s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);

				if(way_read >=0){
					if(!magic){
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L1D.block[set][way_read].full_addr, ip, type, type, 1);
						ooo_cpu[cpu].L1D.block[set][way_read].used = 1;
					}
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;

					if(!magic){
						way_fill = ooo_cpu[cpu].L1D.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L1D.block[set], ip, pd2s, type);
						ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_fill, pd2s, ip, ooo_cpu[cpu].L1D.block[set][way_fill].full_addr, type, 0);

						if (ooo_cpu[cpu].L1D.block[set][way_fill].valid == 0)
							ooo_cpu[cpu].L1D.block[set][way_fill].valid = 1;

						ooo_cpu[cpu].L1D.block[set][way_fill].dirty = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].prefetch = 0;
						ooo_cpu[cpu].L1D.block[set][way_fill].used = 0;

						ooo_cpu[cpu].L1D.block[set][way_fill].tag = pd2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].address = pd2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].full_addr = pd2s;
						ooo_cpu[cpu].L1D.block[set][way_fill].data = 55;
						ooo_cpu[cpu].L1D.block[set][way_fill].cpu = cpu;
					}

					set = ooo_cpu[cpu].L2C.get_set(pd2s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][1]++;
						if(!magic){
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L2C.block[set][way_read].full_addr, ip, type, type, 1);
							ooo_cpu[cpu].L2C.block[set][way_read].used = 1;
						}
						cstall+=L2C_LATENCY;
					}
					else{
						cstall += L2C_LATENCY;

						if(!magic){
							way_fill = ooo_cpu[cpu].L2C.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L2C.block[set], ip, pd2s, type);
							ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_fill, pd2s, ip, ooo_cpu[cpu].L2C.block[set][way_fill].full_addr, type, 0);

							if (ooo_cpu[cpu].L2C.block[set][way_fill].valid == 0)
								ooo_cpu[cpu].L2C.block[set][way_fill].valid = 1;

							ooo_cpu[cpu].L2C.block[set][way_fill].dirty = 0;
							ooo_cpu[cpu].L2C.block[set][way_fill].prefetch = 0;
							ooo_cpu[cpu].L2C.block[set][way_fill].used = 0;

							ooo_cpu[cpu].L2C.block[set][way_fill].tag = pd2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].address = pd2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].full_addr = pd2s;
							ooo_cpu[cpu].L2C.block[set][way_fill].data = 55;
							ooo_cpu[cpu].L2C.block[set][way_fill].cpu = cpu;
						}

						set = uncore.LLC.get_set(pd2s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][2]++;
							if(!magic){
								uncore.LLC.llc_update_replacement_state(cpu, set, way_read, uncore.LLC.block[set][way_read].full_addr, ip, 0, type, 1);
								uncore.LLC.block[set][way_read].used = 1;
							}
							cstall += LLC_LATENCY;
						}
						else{
							cstall += LLC_LATENCY;

							if(!magic){
								way_fill = uncore.LLC.llc_find_victim(cpu, instr_id, set, uncore.LLC.block[set], ip, pd2s, type);
								uncore.LLC.llc_update_replacement_state(cpu, set, way_fill, pd2s, ip, uncore.LLC.block[set][way_fill].full_addr, type, 0);

								if (uncore.LLC.block[set][way_fill].valid == 0)
									uncore.LLC.block[set][way_fill].valid = 1;

								uncore.LLC.block[set][way_fill].dirty = 0;
								uncore.LLC.block[set][way_fill].prefetch = 0;
								uncore.LLC.block[set][way_fill].used = 0;

								uncore.LLC.block[set][way_fill].tag = pd2s;
								uncore.LLC.block[set][way_fill].address = pd2s;
								uncore.LLC.block[set][way_fill].full_addr = pd2s;
								uncore.LLC.block[set][way_fill].data = 55;
								uncore.LLC.block[set][way_fill].cpu = cpu;
							}
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][3]++;
							cstall += 200;
						}
					}
				}
				if(debug)
					cout << "STALL_pd: " << cstall << endl;
			}
		}

		if(0 == 0 && ideal!=1){
			if(debug)
				cout << "PT MISS" << endl;

			PACKET search_packet;
			search_packet.address = pt2s;

			set = ooo_cpu[cpu].L1D.get_set(pt2s);
			way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);

			if(way_read >=0){
				if(!magic){
					ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L1D.block[set][way_read].full_addr, ip, type, type, 1);
					ooo_cpu[cpu].L1D.block[set][way_read].used = 1;
				}
				cstall += L1D_LATENCY;
			}
			else{
				cstall += L1D_LATENCY;

				if(!magic){
					way_fill = ooo_cpu[cpu].L1D.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L1D.block[set], ip, pt2s, type);
					ooo_cpu[cpu].L1D.update_replacement_state(cpu, set, way_fill, pt2s, ip, ooo_cpu[cpu].L1D.block[set][way_fill].full_addr, type, 0);

					if (ooo_cpu[cpu].L1D.block[set][way_fill].valid == 0)
						ooo_cpu[cpu].L1D.block[set][way_fill].valid = 1;

					ooo_cpu[cpu].L1D.block[set][way_fill].dirty = 0;
					ooo_cpu[cpu].L1D.block[set][way_fill].prefetch = 0;
					ooo_cpu[cpu].L1D.block[set][way_fill].used = 0;

					ooo_cpu[cpu].L1D.block[set][way_fill].tag = pt2s;
					ooo_cpu[cpu].L1D.block[set][way_fill].address = pt2s;
					ooo_cpu[cpu].L1D.block[set][way_fill].full_addr = pt2s;
					ooo_cpu[cpu].L1D.block[set][way_fill].data = 55;
					ooo_cpu[cpu].L1D.block[set][way_fill].cpu = cpu;
				}

				set = ooo_cpu[cpu].L2C.get_set(pt2s);
				way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
				if(way_read >=0){
					if (iflag)
						ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][1]++;
					if(!magic){
						ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_read, ooo_cpu[cpu].L2C.block[set][way_read].full_addr, ip, type, type, 1);
						ooo_cpu[cpu].L2C.block[set][way_read].used = 1;
					}
					cstall+=L2C_LATENCY;
				}
				else{
					cstall += L2C_LATENCY;

					if(!magic){
						way_fill = ooo_cpu[cpu].L2C.find_victim(cpu, instr_id, set, ooo_cpu[cpu].L2C.block[set], ip, pt2s, type);
						ooo_cpu[cpu].L2C.update_replacement_state(cpu, set, way_fill, pt2s, ip, ooo_cpu[cpu].L2C.block[set][way_fill].full_addr, type, 0);

						if (ooo_cpu[cpu].L2C.block[set][way_fill].valid == 0)
							ooo_cpu[cpu].L2C.block[set][way_fill].valid = 1;

						ooo_cpu[cpu].L2C.block[set][way_fill].dirty = 0;
						ooo_cpu[cpu].L2C.block[set][way_fill].prefetch = 0;
						ooo_cpu[cpu].L2C.block[set][way_fill].used = 0;

						ooo_cpu[cpu].L2C.block[set][way_fill].tag = pt2s;
						ooo_cpu[cpu].L2C.block[set][way_fill].address = pt2s;
						ooo_cpu[cpu].L2C.block[set][way_fill].full_addr = pt2s;
						ooo_cpu[cpu].L2C.block[set][way_fill].data = 55;
						ooo_cpu[cpu].L2C.block[set][way_fill].cpu = cpu;
					}

					set = uncore.LLC.get_set(pt2s);
					way_read = uncore.LLC.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][2]++;
						if(!magic){
							uncore.LLC.llc_update_replacement_state(cpu, set, way_read, uncore.LLC.block[set][way_read].full_addr, ip, 0, type, 1);
							uncore.LLC.block[set][way_read].used = 1;
						}
						cstall += LLC_LATENCY;
					}
					else{
						cstall += LLC_LATENCY;

						if(!magic){
							way_fill = uncore.LLC.llc_find_victim(cpu, instr_id, set, uncore.LLC.block[set], ip, pt2s, type);
							uncore.LLC.llc_update_replacement_state(cpu, set, way_fill, pt2s, ip, uncore.LLC.block[set][way_fill].full_addr, type, 0);

							if (uncore.LLC.block[set][way_fill].valid == 0)
								uncore.LLC.block[set][way_fill].valid = 1;

							uncore.LLC.block[set][way_fill].dirty = 0;
							uncore.LLC.block[set][way_fill].prefetch = 0;
							uncore.LLC.block[set][way_fill].used = 0;

							uncore.LLC.block[set][way_fill].tag = pt2s;
							uncore.LLC.block[set][way_fill].address = pt2s;
							uncore.LLC.block[set][way_fill].full_addr = pt2s;
							uncore.LLC.block[set][way_fill].data = 55;
							uncore.LLC.block[set][way_fill].cpu = cpu;
						}

						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][3]++;
						cstall += 200;
					}
				}
			}
		}

		if (PAGE_TABLE_LATENCY != 0){
			if(magic != 2 && ideal != 1)
				stall_cycle[cpu] = current_core_cycle[cpu] + cstall;

		}
	}
	return make_pair(pa,cstall);
}

void add_stall_prefetch(int cycles, uint32_t cpu){
	stall_cycle[cpu] = current_core_cycle[cpu] + cycles;
}

uint64_t va_to_pa_prefetch(uint32_t cpu, uint64_t va, uint64_t unique_vpage){
#ifdef SANITY_CHECK
	if (va == 0)
		assert(0);
#endif
	uint64_t high_bit_mask = rotr64(cpu, lg2(NUM_CPUS)),
		 unique_va = va | high_bit_mask;
	uint64_t vpage = unique_vpage | high_bit_mask,
		 voffset = unique_va & ((1<<LOG2_PAGE_SIZE) - 1);

	uint64_t random_ppage;

	map <uint64_t, uint64_t>::iterator pr = page_table.begin();
	map <uint64_t, uint64_t>::iterator ppage_check = inverse_table.begin();


	map <uint64_t, uint64_t>::iterator cl_check = unique_cl[cpu].find(unique_va >> LOG2_BLOCK_SIZE);
	if (cl_check == unique_cl[cpu].end()) { 
		unique_cl[cpu].insert(make_pair(unique_va >> LOG2_BLOCK_SIZE, 0));
		num_cl[cpu]++;
	}
	else
		cl_check->second++;

	pr = page_table.find(vpage);
	if (pr == page_table.end()){
		return 0;
	}
	else{
		uint64_t ppage = pr->second;
		uint64_t pa = ppage << LOG2_PAGE_SIZE;
		pa |= voffset;
		return pa;
	}
} 

int mmu_cache_prefetch_search(uint32_t cpu, uint64_t vpage, int swap, uint64_t instr_id, uint64_t ip, int type, int iflag){

	int mmu_hit[3] = {0,0,0};
	uint64_t compare_pml4, compare_pdp, compare_pd;

	uint64_t cstall = 2;
	ooo_cpu[cpu].STLB.mmu_timer++;

	mmu_hit[0] = ooo_cpu[cpu].STLB.search_pml4(compare_pml4);
	mmu_hit[1] = ooo_cpu[cpu].STLB.search_pdp(compare_pdp);
	if(LOG2_PAGE_SIZE == 12)
		mmu_hit[2] = ooo_cpu[cpu].STLB.search_pd(compare_pd);

	if (mmu_hit[2] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_prefetch_hits[2]++;
	else if (mmu_hit[1] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_prefetch_hits[1]++;
	else if (mmu_hit[0] && iflag)
		ooo_cpu[cpu].STLB.mmu_cache_prefetch_hits[0]++;
	else if (iflag)
		ooo_cpu[cpu].STLB.mmu_cache_prefetch_hits[3]++;
	else
		int unknown = 0;

	if (swap == 0){
		ooo_cpu[cpu].STLB.lru_pml4(ooo_cpu[cpu].STLB.mmu_timer, compare_pml4);
		ooo_cpu[cpu].STLB.lru_pdp(ooo_cpu[cpu].STLB.mmu_timer, compare_pdp);
		if(LOG2_PAGE_SIZE == 12)
			ooo_cpu[cpu].STLB.lru_pd(ooo_cpu[cpu].STLB.mmu_timer, compare_pd);

		uint64_t cr3 = 0x200000;

		uint64_t pt_index, pd_index, pdp_index, pml4_index;

		if(LOG2_PAGE_SIZE == 12){
			pt_index   = (vpage & 0x00000000001ff);
			pd_index   = ((vpage>>9) & 0x00000000001ff);
			pdp_index  = ((vpage>>18) & 0x00000000001ff);
			pml4_index = ((vpage>>27) & 0x00000000001ff);
		}
		else{
			pt_index   = (vpage & 0x00000000001ff);
			pdp_index  = ((vpage>>9) & 0x00000000001ff);
			pml4_index = ((vpage>>18) & 0x00000000001ff);
		}

		uint64_t pml42s, pdp2s, pd2s, pt2s;

		if(LOG2_PAGE_SIZE == 12){
			pml42s = cr3 + pml4_index * 8;

			pdp2s  = cr3 + 512 * 8 \
				 + pml4_index * 512 * 8 \
				 + pdp_index * 8;

			pd2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 8 \
			       + pdp_index * 512 * 8 \
			       + pd_index * 8;

			pt2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + 512 * 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 512 * 8 \
			       + pdp_index * 512 * 512 * 8 \
			       + pd_index * 512 * 8\
			       + pt_index * 8;
		}
		else{
			pml42s = cr3 + pml4_index * 8;

			pdp2s  = cr3 + 512 * 8 \
				 + pml4_index * 512 * 8 \
				 + pdp_index * 8;

			pt2s = cr3 + 512 * 8 \
			       + 512 * 512 * 8 \
			       + pml4_index * 512 * 512 * 8 \
			       + pdp_index * 512 * 8 \
			       + pt_index * 8;
		}

		uint32_t set, way_fill;
		int way_read;

		int debug = 0;

		if(debug)
			cout << "\nNEW PAGE WALK " << endl;
		int asap = 0;
		if(!asap){
			if(mmu_hit[0] == 0){
				PACKET search_packet;
				search_packet.address = pml42s;

				set = ooo_cpu[cpu].L1D.get_set(pml42s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);
				if(way_read >=0){
					if (iflag)
						ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][0]++;
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;

					set = ooo_cpu[cpu].L2C.get_set(pml42s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][1]++;
						cstall += L2C_LATENCY;
					}
					else{
						cstall += L2C_LATENCY;

						set = uncore.LLC.get_set(pml42s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][2]++;
							cstall += LLC_LATENCY;
						}
						else{
							cstall += LLC_LATENCY;

							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[0][3]++;
							cstall += 200;
						}
					}
				}
			}

			if(mmu_hit[1] == 0){
				PACKET search_packet;
				search_packet.address = pdp2s;

				set = ooo_cpu[cpu].L1D.get_set(pdp2s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);
				if(way_read >=0){
					if (iflag)
						ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][0]++;
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;
					set = ooo_cpu[cpu].L2C.get_set(pdp2s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][1]++;
						cstall+=L2C_LATENCY;
					}
					else{
						cstall += L2C_LATENCY;

						set = uncore.LLC.get_set(pdp2s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][2]++;
							cstall += LLC_LATENCY;
						}
						else{
							cstall += LLC_LATENCY;

							cstall += 200;
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[1][3]++;
						}
					}
				}
			}

			if((mmu_hit[2] == 0) && (LOG2_PAGE_SIZE==12)){
				PACKET search_packet;
				search_packet.address = pd2s;

				set = ooo_cpu[cpu].L1D.get_set(pd2s);
				way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);
				if(way_read >=0){
					if (iflag)
						ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][0]++;
					cstall += L1D_LATENCY;
				}
				else{
					cstall += L1D_LATENCY;

					set = ooo_cpu[cpu].L2C.get_set(pd2s);
					way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][1]++;
						cstall+=L2C_LATENCY;
					}
					else{
						cstall += L2C_LATENCY;

						set = uncore.LLC.get_set(pd2s);
						way_read = uncore.LLC.check_hit(&search_packet);
						if(way_read >=0){
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][2]++;
							cstall += LLC_LATENCY;
						}
						else{
							cstall += LLC_LATENCY;

							cstall += 200;
							if (iflag)
								ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[2][3]++;
						}
					}
				}
			}
		}
		if(0 == 0){
			PACKET search_packet;
			search_packet.address = pt2s;

			set = ooo_cpu[cpu].L1D.get_set(pt2s);
			way_read = ooo_cpu[cpu].L1D.check_hit(&search_packet);
			if(way_read >=0){
				if (iflag)
					ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][0]++;
					cstall += L1D_LATENCY;
			}
			else{
				cstall += L1D_LATENCY;

				set = ooo_cpu[cpu].L2C.get_set(pt2s);
				way_read = ooo_cpu[cpu].L2C.check_hit(&search_packet);
				if(way_read >=0){
					if (iflag)
						ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][1]++;
					cstall+=L2C_LATENCY;
				}
				else{
					cstall += L2C_LATENCY;

					set = uncore.LLC.get_set(pt2s);
					way_read = uncore.LLC.check_hit(&search_packet);
					if(way_read >=0){
						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][2]++;
						cstall += LLC_LATENCY;
					}
					else{
						cstall += LLC_LATENCY;

						if (iflag)
							ooo_cpu[cpu].STLB.pagetable_mr_hit_ratio[3][3]++;
						cstall += 200;
					}
				}
			}
		}
	}

	return cstall;
}

void cpu_l1i_prefetcher_cache_operate(uint32_t cpu_num, uint64_t v_addr, uint8_t cache_hit, uint8_t prefetch_hit)
{
	ooo_cpu[cpu_num].l1i_prefetcher_cache_operate(v_addr, cache_hit, prefetch_hit);
}

void cpu_l1i_prefetcher_cache_fill(uint32_t cpu_num, uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr)
{
	ooo_cpu[cpu_num].l1i_prefetcher_cache_fill(addr, set, way, prefetch, evicted_addr);
}

int main(int argc, char** argv)
{
	// interrupt signal hanlder
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = signal_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	cout << endl << "*** ChampSim Multicore Out-of-Order Simulator ***" << endl << endl;

	// initialize knobs
	uint8_t show_heartbeat = 1;

	uint32_t seed_number = 0;

	// check to see if knobs changed using getopt_long()
	int c;
	while (1) {
		static struct option long_options[] =
		{
			{"warmup_instructions", required_argument, 0, 'w'},
			{"simulation_instructions", required_argument, 0, 'i'},
			{"hide_heartbeat", no_argument, 0, 'h'},
			{"cloudsuite", no_argument, 0, 'c'},
			{"low_bandwidth",  no_argument, 0, 'b'},
			{"traces",  no_argument, 0, 't'},
			{0, 0, 0, 0}      
		};

		int option_index = 0;

		c = getopt_long_only(argc, argv, "wihsb", long_options, &option_index);

		// no more option characters
		if (c == -1)
			break;

		int traces_encountered = 0;

		switch(c) {
			case 'w':
				warmup_instructions = atol(optarg);
				break;
			case 'i':
				simulation_instructions = atol(optarg);
				break;
			case 'h':
				show_heartbeat = 0;
				break;
			case 'c':
				knob_cloudsuite = 1;
				MAX_INSTR_DESTINATIONS = NUM_INSTR_DESTINATIONS_SPARC;
				break;
			case 'b':
				knob_low_bandwidth = 1;
				break;
			case 't':
				traces_encountered = 1;
				break;
			default:
				abort();
		}

		if (traces_encountered == 1)
			break;
	}

	// consequences of knobs
	cout << "Warmup Instructions: " << warmup_instructions << endl;
	cout << "Simulation Instructions: " << simulation_instructions << endl;
	//cout << "Scramble Loads: " << (knob_scramble_loads ? "ture" : "false") << endl;
	cout << "Number of CPUs: " << NUM_CPUS << endl;
	cout << "LLC sets: " << LLC_SET << endl;
	cout << "LLC ways: " << LLC_WAY << endl;

	if (knob_low_bandwidth)
		DRAM_MTPS = DRAM_IO_FREQ/4;
	else
		DRAM_MTPS = DRAM_IO_FREQ;

	// DRAM access latency
	tRP  = (uint32_t)((1.0 * tRP_DRAM_NANOSECONDS  * CPU_FREQ) / 1000); 
	tRCD = (uint32_t)((1.0 * tRCD_DRAM_NANOSECONDS * CPU_FREQ) / 1000); 
	tCAS = (uint32_t)((1.0 * tCAS_DRAM_NANOSECONDS * CPU_FREQ) / 1000); 

	// default: 16 = (64 / 8) * (3200 / 1600)
	// it takes 16 CPU cycles to tranfser 64B cache block on a 8B (64-bit) bus 
	// note that dram burst length = BLOCK_SIZE/DRAM_CHANNEL_WIDTH
	DRAM_DBUS_RETURN_TIME = (BLOCK_SIZE / DRAM_CHANNEL_WIDTH) * (CPU_FREQ / DRAM_MTPS);

	printf("Off-chip DRAM Size: %u MB Channels: %u Width: %u-bit Data Rate: %u MT/s\n",
			DRAM_SIZE, DRAM_CHANNELS, 8*DRAM_CHANNEL_WIDTH, DRAM_MTPS);

	// end consequence of knobs

	// search through the argv for "-traces"
	int found_traces = 0;
	int count_traces = 0;
	cout << endl;
	for (int i=0; i<argc; i++) {
		if (found_traces)
		{
			printf("CPU %d runs %s\n", count_traces, argv[i]);

			sprintf(ooo_cpu[count_traces].trace_string, "%s", argv[i]);

			std::string full_name(argv[i]);
			std::string last_dot = full_name.substr(full_name.find_last_of("."));

			std::string fmtstr;
			std::string decomp_program;
			if (full_name.substr(0,4) == "http")
			{
				// Check file exists
				char testfile_command[4096];
				sprintf(testfile_command, "wget -q --spider %s", argv[i]);
				FILE *testfile = popen(testfile_command, "r");
				if (pclose(testfile))
				{
					std::cerr << "TRACE FILE NOT FOUND" << std::endl;
					assert(0);
				}
				fmtstr = "wget -qO- %2$s | %1$s -dc";
			}
			else
			{
				std::ifstream testfile(argv[i]);
				if (!testfile.good())
				{
					std::cerr << "TRACE FILE NOT FOUND" << std::endl;
					assert(0);
				}
				fmtstr = "%1$s -dc %2$s";
			}

			if (last_dot[1] == 'g') // gzip format
				decomp_program = "gzip";
			else if (last_dot[1] == 'x') // xz
				decomp_program = "xz";
			else {
				std::cout << "ChampSim does not support traces other than gz or xz compression!" << std::endl;
				assert(0);
			}

			sprintf(ooo_cpu[count_traces].gunzip_command, fmtstr.c_str(), decomp_program.c_str(), argv[i]);

			char *pch[100];
			int count_str = 0;
			pch[0] = strtok (argv[i], " /,.-");
			while (pch[count_str] != NULL) {
				//printf ("%s %d\n", pch[count_str], count_str);
				count_str++;
				pch[count_str] = strtok (NULL, " /,.-");
			}

			//printf("max count_str: %d\n", count_str);
			//printf("application: %s\n", pch[count_str-3]);

			int j = 0;
			while (pch[count_str-3][j] != '\0') {
				seed_number += pch[count_str-3][j];
				//printf("%c %d %d\n", pch[count_str-3][j], j, seed_number);
				j++;
			}

			ooo_cpu[count_traces].trace_file = popen(ooo_cpu[count_traces].gunzip_command, "r");
			if (ooo_cpu[count_traces].trace_file == NULL) {
				printf("\n*** Trace file not found: %s ***\n\n", argv[i]);
				assert(0);
			}

			count_traces++;
			if (count_traces > NUM_CPUS) {
				printf("\n*** Too many traces for the configured number of cores ***\n\n");
				assert(0);
			}
		}
		else if(strcmp(argv[i],"-traces") == 0) {
			found_traces = 1;
		}
	}

	if (count_traces != NUM_CPUS) {
		printf("\n*** Not enough traces for the configured number of cores ***\n\n");
		assert(0);
	}
	// end trace file setup

	// TODO: can we initialize these variables from the class constructor?
	srand(seed_number);
	champsim_seed = seed_number;
	for (int i=0; i<NUM_CPUS; i++) {

		ooo_cpu[i].cpu = i; 
		ooo_cpu[i].warmup_instructions = warmup_instructions;
		ooo_cpu[i].simulation_instructions = simulation_instructions;
		ooo_cpu[i].begin_sim_cycle = 0; 
		ooo_cpu[i].begin_sim_instr = warmup_instructions;

		// ROB
		ooo_cpu[i].ROB.cpu = i;

		// BRANCH PREDICTOR
		ooo_cpu[i].initialize_branch_predictor();

		// TLBs
		ooo_cpu[i].ITLB.cpu = i;
		ooo_cpu[i].ITLB.cache_type = IS_ITLB;
		ooo_cpu[i].ITLB.MAX_READ = 1;
		ooo_cpu[i].ITLB.fill_level = FILL_L1;
		ooo_cpu[i].ITLB.extra_interface = &ooo_cpu[i].L1I;
		ooo_cpu[i].ITLB.lower_level = &ooo_cpu[i].STLB; 

		ooo_cpu[i].DTLB.cpu = i;
		ooo_cpu[i].DTLB.cache_type = IS_DTLB;
		ooo_cpu[i].DTLB.MAX_READ = (2 > MAX_READ_PER_CYCLE) ? MAX_READ_PER_CYCLE : 2;
		//ooo_cpu[i].DTLB.MAX_READ = 2;
		ooo_cpu[i].DTLB.fill_level = FILL_L1;
		ooo_cpu[i].DTLB.extra_interface = &ooo_cpu[i].L1D;
		ooo_cpu[i].DTLB.lower_level = &ooo_cpu[i].STLB;

		ooo_cpu[i].STLB.cpu = i;
		ooo_cpu[i].STLB.cache_type = IS_STLB;
		ooo_cpu[i].STLB.MAX_READ = 1;
		ooo_cpu[i].STLB.fill_level = FILL_L2;
		ooo_cpu[i].STLB.upper_level_icache[i] = &ooo_cpu[i].ITLB;
		ooo_cpu[i].STLB.upper_level_dcache[i] = &ooo_cpu[i].DTLB;
		ooo_cpu[i].STLB.stlb_prefetcher_initialize();

		// PRIVATE CACHE
		ooo_cpu[i].L1I.cpu = i;
		ooo_cpu[i].L1I.cache_type = IS_L1I;
		//ooo_cpu[i].L1I.MAX_READ = (FETCH_WIDTH > MAX_READ_PER_CYCLE) ? MAX_READ_PER_CYCLE : FETCH_WIDTH;
		ooo_cpu[i].L1I.MAX_READ = 2;
		ooo_cpu[i].L1I.fill_level = FILL_L1;
		ooo_cpu[i].L1I.lower_level = &ooo_cpu[i].L2C; 
		ooo_cpu[i].l1i_prefetcher_initialize();
		ooo_cpu[i].L1I.l1i_prefetcher_cache_operate = cpu_l1i_prefetcher_cache_operate;
		ooo_cpu[i].L1I.l1i_prefetcher_cache_fill = cpu_l1i_prefetcher_cache_fill;

		ooo_cpu[i].L1D.cpu = i;
		ooo_cpu[i].L1D.cache_type = IS_L1D;
		ooo_cpu[i].L1D.MAX_READ = (2 > MAX_READ_PER_CYCLE) ? MAX_READ_PER_CYCLE : 2;
		ooo_cpu[i].L1D.fill_level = FILL_L1;
		ooo_cpu[i].L1D.lower_level = &ooo_cpu[i].L2C; 
		ooo_cpu[i].L1D.l1d_prefetcher_initialize();

		ooo_cpu[i].L2C.cpu = i;
		ooo_cpu[i].L2C.cache_type = IS_L2C;
		ooo_cpu[i].L2C.fill_level = FILL_L2;
		ooo_cpu[i].L2C.upper_level_icache[i] = &ooo_cpu[i].L1I;
		ooo_cpu[i].L2C.upper_level_dcache[i] = &ooo_cpu[i].L1D;
		ooo_cpu[i].L2C.lower_level = &uncore.LLC;
		ooo_cpu[i].L2C.l2c_prefetcher_initialize();

		// SHARED CACHE
		uncore.LLC.cache_type = IS_LLC;
		uncore.LLC.fill_level = FILL_LLC;
		uncore.LLC.MAX_READ = NUM_CPUS;
		uncore.LLC.upper_level_icache[i] = &ooo_cpu[i].L2C;
		uncore.LLC.upper_level_dcache[i] = &ooo_cpu[i].L2C;
		uncore.LLC.lower_level = &uncore.DRAM;

		// OFF-CHIP DRAM
		uncore.DRAM.fill_level = FILL_DRAM;
		uncore.DRAM.upper_level_icache[i] = &uncore.LLC;
		uncore.DRAM.upper_level_dcache[i] = &uncore.LLC;
		for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
			uncore.DRAM.RQ[i].is_RQ = 1;
			uncore.DRAM.WQ[i].is_WQ = 1;
		}

		warmup_complete[i] = 0;
		//all_warmup_complete = NUM_CPUS;
		simulation_complete[i] = 0;
		current_core_cycle[i] = 0;
		stall_cycle[i] = 0;

		previous_ppage = 0;
		num_adjacent_page = 0;
		num_cl[i] = 0;
		allocated_pages = 0;
		num_page[i] = 0;
		minor_fault[i] = 0;
		major_fault[i] = 0;
	}

	uncore.LLC.llc_initialize_replacement();
	uncore.LLC.llc_prefetcher_initialize();

	// simulation entry point
	start_time = time(NULL);
	uint8_t run_simulation = 1;
	while (run_simulation) {

		uint64_t elapsed_second = (uint64_t)(time(NULL) - start_time),
			 elapsed_minute = elapsed_second / 60,
			 elapsed_hour = elapsed_minute / 60;
		elapsed_minute -= elapsed_hour*60;
		elapsed_second -= (elapsed_hour*3600 + elapsed_minute*60);

		for (int i=0; i<NUM_CPUS; i++) {
			// proceed one cycle
			current_core_cycle[i]++;

			//cout << "Trying to process instr_id: " << ooo_cpu[i].instr_unique_id << " fetch_stall: " << +ooo_cpu[i].fetch_stall;
			//cout << " stall_cycle: " << stall_cycle[i] << " current: " << current_core_cycle[i] << endl;

			// core might be stalled due to page fault or branch misprediction
			if (stall_cycle[i] <= current_core_cycle[i]) {

				// retire
				if ((ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].executed == COMPLETED) && (ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].event_cycle <= current_core_cycle[i]))
					ooo_cpu[i].retire_rob();

				// complete 
				ooo_cpu[i].update_rob();

				// schedule
				uint32_t schedule_index = ooo_cpu[i].ROB.next_schedule;
				if ((ooo_cpu[i].ROB.entry[schedule_index].scheduled == 0) && (ooo_cpu[i].ROB.entry[schedule_index].event_cycle <= current_core_cycle[i]))
					ooo_cpu[i].schedule_instruction();
				// execute
				ooo_cpu[i].execute_instruction();

				ooo_cpu[i].update_rob();

				// memory operation
				ooo_cpu[i].schedule_memory_instruction();
				ooo_cpu[i].execute_memory_instruction();

				ooo_cpu[i].update_rob();

				// decode
				if(ooo_cpu[i].DECODE_BUFFER.occupancy > 0)
				{
					ooo_cpu[i].decode_and_dispatch();
				}

				// fetch
				ooo_cpu[i].fetch_instruction();

				// read from trace
				if ((ooo_cpu[i].IFETCH_BUFFER.occupancy < ooo_cpu[i].IFETCH_BUFFER.SIZE) && (ooo_cpu[i].fetch_stall == 0))
				{
					ooo_cpu[i].read_from_trace();
				}
			}

			// heartbeat information
			if (show_heartbeat && (ooo_cpu[i].num_retired >= ooo_cpu[i].next_print_instruction)) {
				float cumulative_ipc;
				if (warmup_complete[i])
					cumulative_ipc = (1.0*(ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr)) / (current_core_cycle[i] - ooo_cpu[i].begin_sim_cycle);
				else
					cumulative_ipc = (1.0*ooo_cpu[i].num_retired) / current_core_cycle[i];
				float heartbeat_ipc = (1.0*ooo_cpu[i].num_retired - ooo_cpu[i].last_sim_instr) / (current_core_cycle[i] - ooo_cpu[i].last_sim_cycle);

				cout << "Heartbeat CPU " << i << " instructions: " << ooo_cpu[i].num_retired << " cycles: " << current_core_cycle[i];
				cout << " heartbeat IPC: " << heartbeat_ipc << " cumulative IPC: " << cumulative_ipc; 
				cout << " (Simulation time: " << elapsed_hour << " hr " << elapsed_minute << " min " << elapsed_second << " sec) " << endl;
				ooo_cpu[i].next_print_instruction += STAT_PRINTING_PERIOD;

				ooo_cpu[i].last_sim_instr = ooo_cpu[i].num_retired;
				ooo_cpu[i].last_sim_cycle = current_core_cycle[i];
			}

			// check for deadlock
			if (ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].ip && (ooo_cpu[i].ROB.entry[ooo_cpu[i].ROB.head].event_cycle + DEADLOCK_CYCLE) <= current_core_cycle[i])
				print_deadlock(i);

			// check for warmup
			// warmup complete
			if ((warmup_complete[i] == 0) && (ooo_cpu[i].num_retired > warmup_instructions)) {
				warmup_complete[i] = 1;
				all_warmup_complete++;
			}
			if (all_warmup_complete == NUM_CPUS) { // this part is called only once when all cores are warmed up
				all_warmup_complete++;
				finish_warmup();
			}

			/*
			   if (all_warmup_complete == 0) { 
			   all_warmup_complete = 1;
			   finish_warmup();
			   }
			   if (ooo_cpu[1].num_retired > 0)
			   warmup_complete[1] = 1;
			   */

			// simulation complete
			if ((all_warmup_complete > NUM_CPUS) && (simulation_complete[i] == 0) && (ooo_cpu[i].num_retired >= (ooo_cpu[i].begin_sim_instr + ooo_cpu[i].simulation_instructions))) {
				simulation_complete[i] = 1;
				ooo_cpu[i].finish_sim_instr = ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr;
				ooo_cpu[i].finish_sim_cycle = current_core_cycle[i] - ooo_cpu[i].begin_sim_cycle;

				cout << "Finished CPU " << i << " instructions: " << ooo_cpu[i].finish_sim_instr << " cycles: " << ooo_cpu[i].finish_sim_cycle;
				cout << " cumulative IPC: " << ((float) ooo_cpu[i].finish_sim_instr / ooo_cpu[i].finish_sim_cycle);
				cout << " (Simulation time: " << elapsed_hour << " hr " << elapsed_minute << " min " << elapsed_second << " sec) " << endl;

				record_roi_stats(i, &ooo_cpu[i].ITLB);
				record_roi_stats(i, &ooo_cpu[i].DTLB);
				record_roi_stats(i, &ooo_cpu[i].L1D);
				record_roi_stats(i, &ooo_cpu[i].L1I);
				record_roi_stats(i, &ooo_cpu[i].L2C);
				record_roi_stats(i, &ooo_cpu[i].STLB);
				record_roi_stats(i, &uncore.LLC);

				all_simulation_complete++;
			}

			if (all_simulation_complete == NUM_CPUS)
				run_simulation = 0;
		}

		// TODO: should it be backward?
		uncore.DRAM.operate();
		uncore.LLC.operate();
	}

	uint64_t elapsed_second = (uint64_t)(time(NULL) - start_time),
		 elapsed_minute = elapsed_second / 60,
		 elapsed_hour = elapsed_minute / 60;
	elapsed_minute -= elapsed_hour*60;
	elapsed_second -= (elapsed_hour*3600 + elapsed_minute*60);

	cout << endl << "ChampSim completed all CPUs" << endl;
	if (NUM_CPUS > 1) {
		cout << endl << "Total Simulation Statistics (not including warmup)" << endl;
		for (uint32_t i=0; i<NUM_CPUS; i++) {
			cout << endl << "CPU " << i << " cumulative IPC: " << (float) (ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr) / (current_core_cycle[i] - ooo_cpu[i].begin_sim_cycle); 
			cout << " instructions: " << ooo_cpu[i].num_retired - ooo_cpu[i].begin_sim_instr << " cycles: " << current_core_cycle[i] - ooo_cpu[i].begin_sim_cycle << endl;
#ifndef CRC2_COMPILE
			print_sim_stats(i, &ooo_cpu[i].L1D);
			print_sim_stats(i, &ooo_cpu[i].L1I);
			print_sim_stats(i, &ooo_cpu[i].L2C);
			print_sim_stats(i, &ooo_cpu[i].STLB);
			ooo_cpu[i].l1i_prefetcher_final_stats();
			ooo_cpu[i].L1D.l1d_prefetcher_final_stats();
			ooo_cpu[i].L2C.l2c_prefetcher_final_stats();
			ooo_cpu[i].STLB.stlb_prefetcher_final_stats(ooo_cpu[i].STLB.pf_total_pq, ooo_cpu[i].STLB.pf_hits_pq, ooo_cpu[i].STLB.pf_misses_pq, ooo_cpu[i].STLB.pf_swap, ooo_cpu[i].STLB.pf_dupli, ooo_cpu[i].STLB.pf_free, ooo_cpu[i].STLB.pf_real, ooo_cpu[i].STLB.mmu_cache_demand_hits, ooo_cpu[i].STLB.mmu_cache_prefetch_hits, ooo_cpu[i].STLB.rfhits, ooo_cpu[i].STLB.free_hits, ooo_cpu[i].STLB.pagetable_mr_hit_ratio, ooo_cpu[i].STLB.stlb_misses);
#endif
			print_sim_stats(i, &uncore.LLC);
		}
		uncore.LLC.llc_prefetcher_final_stats();
	}

	cout << endl << "Region of Interest Statistics" << endl;
	for (uint32_t i=0; i<NUM_CPUS; i++) {
		cout << endl << "CPU " << i << " cumulative IPC: " << ((float) ooo_cpu[i].finish_sim_instr / ooo_cpu[i].finish_sim_cycle); 
		cout << " instructions: " << ooo_cpu[i].finish_sim_instr << " cycles: " << ooo_cpu[i].finish_sim_cycle << endl;
#ifndef CRC2_COMPILE
		print_roi_stats(i, &ooo_cpu[i].L1D);
		print_roi_stats(i, &ooo_cpu[i].L1I);
		print_roi_stats(i, &ooo_cpu[i].L2C);
		print_roi_stats(i, &ooo_cpu[i].ITLB);
		print_roi_stats(i, &ooo_cpu[i].DTLB);
		print_roi_stats(i, &ooo_cpu[i].STLB);
#endif
		print_roi_stats(i, &uncore.LLC);
		cout << "Major fault: " << major_fault[i] << " Minor fault: " << minor_fault[i] << endl;
	}

	for (uint32_t i=0; i<NUM_CPUS; i++) {
		ooo_cpu[i].l1i_prefetcher_final_stats();
		ooo_cpu[i].L1D.l1d_prefetcher_final_stats();
		ooo_cpu[i].L2C.l2c_prefetcher_final_stats();
		ooo_cpu[i].STLB.stlb_prefetcher_final_stats(ooo_cpu[i].STLB.pf_total_pq, ooo_cpu[i].STLB.pf_hits_pq, ooo_cpu[i].STLB.pf_misses_pq, ooo_cpu[i].STLB.pf_swap, ooo_cpu[i].STLB.pf_dupli, ooo_cpu[i].STLB.pf_free, ooo_cpu[i].STLB.pf_real, ooo_cpu[i].STLB.mmu_cache_demand_hits, ooo_cpu[i].STLB.mmu_cache_prefetch_hits, ooo_cpu[i].STLB.rfhits, ooo_cpu[i].STLB.free_hits, ooo_cpu[i].STLB.pagetable_mr_hit_ratio, ooo_cpu[i].STLB.stlb_misses);
	}

	uncore.LLC.llc_prefetcher_final_stats();

#ifndef CRC2_COMPILE
	uncore.LLC.llc_replacement_final_stats();
	print_dram_stats();
	print_branch_stats();
#endif

	return 0;
}
