#include "cache.h"
#include "morriganPT.h"
#include <cmath>
#include <vector>   
#include <algorithm>
#include <random>

MARKOV_S1 irip_s1[(int)pow(2.0, ceil(log2(PT_S1_SETS/PT_S1_ASSOC)))][PT_S1_ASSOC];
MARKOV_S2 irip_s2[(int)pow(2.0, ceil(log2(PT_S2_SETS/PT_S2_ASSOC)))][PT_S2_ASSOC];
MARKOV_S4 irip_s4[(int)pow(2.0, ceil(log2(PT_S4_SETS/PT_S4_ASSOC)))][PT_S4_ASSOC];
MARKOV_S8 irip_s8[(int)pow(2.0, ceil(log2(PT_S8_SETS/PT_S8_ASSOC)))][PT_S8_ASSOC];

uint64_t s_timer;

uint64_t previous_vpn;

void show_s1(){
	int x = PT_S1_SETS / PT_S1_ASSOC;
	for(int i=0; i<x; ++i){
		for(int j=0; j<PT_S1_ASSOC; ++j){
			cout << hex << irip_s1[i][j].vpn << ", " << irip_s1[i][j].successor_delta << " | ";
		}
		cout << endl;
	}
}

void show_s2(){
	int x = PT_S2_SETS / PT_S2_ASSOC;
	for(int i=0; i<x; ++i){
		for(int j=0; j<PT_S2_ASSOC; ++j){
			cout << hex << irip_s2[i][j].vpn << ", " << irip_s2[i][j].successor_delta[0] << ", " <<  irip_s2[i][j].successor_delta[1] << " | ";
		}
		cout << endl;
	}
}

void show_s4(){
	int x = PT_S4_SETS / PT_S4_ASSOC;
	for(int i=0; i<x; ++i){
		for(int j=0; j<PT_S4_ASSOC; ++j){
			cout << hex << irip_s4[i][j].vpn << ", " << irip_s4[i][j].successor_delta[0] << ", " <<  irip_s4[i][j].successor_delta[1] << ", " << irip_s4[i][j].successor_delta[2] << ", " << irip_s4[i][j].successor_delta[3] << " | ";
		}
		cout << endl;
	}
}

void show_s8(){
	int x = PT_S8_SETS / PT_S8_ASSOC;
	for(int i=0; i<x; ++i){
		for(int j=0; j<PT_S8_ASSOC; ++j){
			cout << hex << irip_s8[i][j].vpn << ", " << irip_s8[i][j].successor_delta[0] << ", " <<  irip_s8[i][j].successor_delta[1] << ", " << irip_s8[i][j].successor_delta[2] << ", " << irip_s8[i][j].successor_delta[3] << ", " << irip_s8[i][j].successor_delta[4] << ", " <<  irip_s8[i][j].successor_delta[5] << ", " << irip_s8[i][j].successor_delta[6] << ", " <<  irip_s8[i][j].successor_delta[7] << " | ";
		}
		cout << endl;
	}
}

uint32_t hash32( uint32_t a){ 
	a = (a+0x479ab41d) + (a<<8);
	a = (a^0xe4aa10ce) ^ (a>>5);
	a = (a+0x9942f0a6) - (a<<14);
	a = (a^0x5aedd67d) ^ (a>>3);
	a = (a+0x17bea992) + (a<<7);
	return a;
}

int search_markov_table(int index, uint64_t current_vpn, int id){
	if(id == 1){
		for(int i=0; i<PT_S1_ASSOC; ++i){
			if(current_vpn == irip_s1[index][i].vpn)
				return i;
		}
	}
	else if(id == 2){
		for(int i=0; i<PT_S2_ASSOC; ++i){
			if(current_vpn == irip_s2[index][i].vpn)
				return i;
		}
	}
	else if(id == 4){
		for(int i=0; i<PT_S4_ASSOC; ++i){
			if(current_vpn == irip_s4[index][i].vpn)
				return i;
		}
	}
	else if(id == 8){
		for(int i=0; i<PT_S8_ASSOC; ++i){
			if(current_vpn == irip_s8[index][i].vpn)
				return i;
		}
	}
	else
		assert(0);

	return -1; 
}


void reset_frequency(){
	int x = PT_S1_SETS / PT_S1_ASSOC;
	for(int i=0; i<x; i++){
		for(int j=0; j<PT_S1_ASSOC; j++){
			irip_s1[i][j].freq = 0;
		}
	}

	x = PT_S2_SETS / PT_S2_ASSOC;
	for(int i=0; i<x; i++){
		for(int j=0; j<PT_S2_ASSOC; j++){
			irip_s2[i][j].freq = 0;
			irip_s2[i][j].confidence[0]/=2;
			irip_s2[i][j].confidence[1]/=2;
		}
	}

	x = PT_S4_SETS / PT_S4_ASSOC;
	for(int i=0; i<x; i++){
		for(int j=0; j<PT_S4_ASSOC; j++){
			irip_s4[i][j].freq = 0;
			irip_s4[i][j].confidence[0]/=2;
			irip_s4[i][j].confidence[1]/=2;
			irip_s4[i][j].confidence[2]/=2;
			irip_s4[i][j].confidence[3]/=2;
		}
	}

	x = PT_S8_SETS / PT_S8_ASSOC;
	for(int i=0; i<x; i++){
		for(int j=0; j<PT_S8_ASSOC; j++){
			irip_s8[i][j].freq = 0;
			irip_s8[i][j].confidence[0]/=2;
			irip_s8[i][j].confidence[1]/=2;
			irip_s8[i][j].confidence[2]/=2;
			irip_s8[i][j].confidence[3]/=2;
			irip_s8[i][j].confidence[4]/=2;
			irip_s8[i][j].confidence[5]/=2;
			irip_s8[i][j].confidence[6]/=2;
			irip_s8[i][j].confidence[7]/=2;
		}
	}
}

int lru_policy(int index, int id){
	if(id == 1){
		uint64_t lru_min = irip_s1[index][0].timestamp;
		int lru_victim = 0;
		for(int m=1; m<PT_S1_ASSOC; m++){
			if(irip_s1[index][m].timestamp < lru_min){
				lru_min = irip_s1[index][m].timestamp;
				lru_victim = m;
			}
		}
		return lru_victim;
	}
	else if(id == 2){
		uint64_t lru_min = irip_s2[index][0].timestamp;
		int lru_victim = 0;
		for(int m=1; m<PT_S2_ASSOC; m++){
			if(irip_s2[index][m].timestamp < lru_min){
				lru_min = irip_s2[index][m].timestamp;
				lru_victim = m;
			}
		}
		return lru_victim;
	}
	else if(id == 4){
		uint64_t lru_min = irip_s4[index][0].timestamp;
		int lru_victim = 0;
		for(int m=1; m<PT_S4_ASSOC; m++){
			if(irip_s4[index][m].timestamp < lru_min){
				lru_min = irip_s4[index][m].timestamp;
				lru_victim = m;
			}
		}
		return lru_victim;
	}
	else if(id == 8){
		uint64_t lru_min = irip_s8[index][0].timestamp;
		int lru_victim = 0;
		for(int m=1; m<PT_S8_ASSOC; m++){
			if(irip_s8[index][m].timestamp < lru_min){
				lru_min = irip_s8[index][m].timestamp;
				lru_victim = m;
			}
		}
		return lru_victim;
	}
	else
		assert(0);
}

int random_policy(int id){
	int range_from = 0;
	int range_to = -100;

	if(id == 1)
		range_to = PT_S1_ASSOC - 1;
	else if(id == 2)
		range_to = PT_S2_ASSOC - 1;
	else if(id == 4)
		range_to = PT_S4_ASSOC - 1;
	else if(id == 8)
		range_to = PT_S8_ASSOC - 1;
	else
		assert(0);

	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(range_from, range_to);

	int random_set = distr(generator);

	return random_set;
}

bool sortcol(const vector<int>& v1, const vector<int>& v2 ){
	return (v1[1] < v2[1]);
}

int lfu_policy(int index, int id){
	if(id == 1){
		vector<vector<int>> lrufreq(PT_S1_ASSOC, vector<int>(2,0));

		for(int j=0; j<PT_S1_ASSOC; j++){
			lrufreq[j][0] = irip_s1[index][j].timestamp;
			lrufreq[j][1] = irip_s1[index][j].freq;
		}

		sort(lrufreq.begin(), lrufreq.end(), sortcol);

		int llimit;
		if(PT_S1_ASSOC > 8)
			llimit = LLIMIT;
		else
			llimit = PT_S1_ASSOC;

		random_device rand_dev;
		mt19937 generator(rand_dev());
		uniform_int_distribution<int>  distr(0, llimit-1);
		int random_set = distr(generator);

		int victim_timestamp = lrufreq[random_set][0];
		for(int m=0; m<PT_S1_ASSOC; m++){
			if(irip_s1[index][m].timestamp == victim_timestamp)
				return m;
		}
	}
	else if(id == 2){
		vector<vector<int>> lrufreq(PT_S2_ASSOC, vector<int>(2,0));

		for(int j=0; j<PT_S2_ASSOC; j++){
			lrufreq[j][0] = irip_s2[index][j].timestamp;
			lrufreq[j][1] = irip_s2[index][j].freq;
		}

		sort(lrufreq.begin(), lrufreq.end(), sortcol);

		int llimit;
		if(PT_S2_ASSOC > 8)
			llimit = LLIMIT;
		else
			llimit = PT_S2_ASSOC;

		random_device rand_dev;
		mt19937 generator(rand_dev());
		uniform_int_distribution<int>  distr(0, llimit-1);
		int random_set = distr(generator);

		int victim_timestamp = lrufreq[random_set][0];
		for(int m=0; m<PT_S2_ASSOC; m++){
			if(irip_s2[index][m].timestamp == victim_timestamp)
				return m;
		}
	}
	else if(id == 4){
		vector<vector<int>> lrufreq(PT_S4_ASSOC, vector<int>(2,0));

		for(int j=0; j<PT_S4_ASSOC; j++){
			lrufreq[j][0] = irip_s4[index][j].timestamp;
			lrufreq[j][1] = irip_s4[index][j].freq;
		}

		sort(lrufreq.begin(), lrufreq.end(), sortcol);

		int llimit;
		if(PT_S4_ASSOC > 8)
			llimit = LLIMIT;
		else
			llimit = PT_S4_ASSOC;

		random_device rand_dev;
		mt19937 generator(rand_dev());
		uniform_int_distribution<int>  distr(0, llimit-1);
		int random_set = distr(generator);

		int victim_timestamp = lrufreq[random_set][0];
		for(int m=0; m<PT_S4_ASSOC; m++){
			if(irip_s4[index][m].timestamp == victim_timestamp)
				return m;
		}
	}
	else if(id == 8){
		vector<vector<int>> lrufreq(PT_S8_ASSOC, vector<int>(2,0));

		for(int j=0; j<PT_S8_ASSOC; j++){
			lrufreq[j][0] = irip_s8[index][j].timestamp;
			lrufreq[j][1] = irip_s8[index][j].freq;
		}

		sort(lrufreq.begin(), lrufreq.end(), sortcol);

		int llimit;
		if(PT_S8_ASSOC > 8)
			llimit = LLIMIT;
		else
			llimit = PT_S8_ASSOC;

		random_device rand_dev;
		mt19937 generator(rand_dev());
		uniform_int_distribution<int>  distr(0, llimit-1);
		int random_set = distr(generator);

		int victim_timestamp = lrufreq[random_set][0];
		for(int m=0; m<PT_S8_ASSOC; m++){
			if(irip_s8[index][m].timestamp == victim_timestamp)
				return m;
		}
	}
	else
		assert(0);
}


void CACHE:: free_prefetching(uint64_t ip, uint64_t addr, int cache_line_position_n, uint64_t pf_addr, int * free_indexes, uint64_t instr_id, int type, int iflag){
	int fi_lim = -100;
	for(int fi=13; fi>=0; fi--){
		fi_lim = free_indexes[fi] + cache_line_position_n;
		if((fi_lim >= 0) && (fi_lim <= 7))
			prefetch_page(ip, addr, pf_addr + free_indexes[fi], FILL_L2, 0, 1, 1, free_indexes[fi], instr_id, type, iflag, 1, 0, 1);
	}   
}

void CACHE::stlb_prefetcher_initialize(){
	cout << "CPU " << cpu << " STLB -- Morrigan I-TLB Prefetcher" << endl;
	s_timer = 0;
	previous_vpn = 0;
}

void CACHE::stlb_prefetcher_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, int answer, int warmup, int * free_indexes, uint64_t instr_id, int iflag){
	if(!iflag)
		assert(0);

	s_timer++; 

	uint64_t current_vpn = addr, partial_vpn = current_vpn & TAG_MASK, clp=0;
	int cache_line_pos, bits, index, way=-1, victim, ignore, hit_level = -1;
	int free_bit=0, free_distance = 0;

	if(answer != -1){
		int ss = PT_S2_SETS / PT_S2_ASSOC;
		for(int i=0; i<ss; ++i){
			for(int j=0; j<PT_S2_ASSOC; ++j){
				for(int k=0; k<2; ++k){
					if(irip_s2[i][j].successor_delta[k] != 0){
						if(partial_vpn == ((irip_s2[i][j].successor_delta[k] + irip_s2[i][j].vpn) & TAG_MASK)){
							if(irip_s2[i][j].confidence[k] < pow(2, CNF_BITS)){
								irip_s2[i][j].confidence[k]++;
								break;
							}
						}
					}
				}
			}
		}

		ss = PT_S4_SETS / PT_S4_ASSOC;
		for(int i=0; i<ss; ++i){
			for(int j=0; j<PT_S4_ASSOC; ++j){
				for(int k=0; k<4; ++k){
					if(irip_s4[i][j].successor_delta[k] != 0){
						if(partial_vpn == ((irip_s4[i][j].successor_delta[k] + irip_s4[i][j].vpn) & TAG_MASK)){
							if(irip_s4[i][j].confidence[k] < pow(2, CNF_BITS)){
								irip_s4[i][j].confidence[k]++;
								break;
							}
						}
					}
				}   
			}
		}

		ss = PT_S8_SETS / PT_S8_ASSOC;
		for(int i=0; i<ss; ++i){
			for(int j=0; j<PT_S8_ASSOC; ++j){
				for(int k=0; k<8; ++k){
					if(irip_s8[i][j].successor_delta[k] != 0){
						if(partial_vpn == ((irip_s8[i][j].successor_delta[k] + irip_s8[i][j].vpn) & TAG_MASK)){
							if(irip_s8[i][j].confidence[k] < pow(2, CNF_BITS)){
								irip_s8[i][j].confidence[k]++;
								break;
							}
						}
					}
				}   
			}
		}
	}

	bits  = ceil(log2(PT_S1_SETS/PT_S1_ASSOC));
	index = hash32(partial_vpn) & ((1 << bits) - 1);
	way   = search_markov_table(index, partial_vpn, 1);

	if(way == -1){
		bits  = ceil(log2(PT_S2_SETS/PT_S2_ASSOC));
		index = hash32(partial_vpn) & ((1 << bits) - 1);
		way   = search_markov_table(index, partial_vpn, 2);
		if(way != -1)
			hit_level = 2;
		else{
			bits  = ceil(log2(PT_S4_SETS/PT_S4_ASSOC));
			index = hash32(partial_vpn) & ((1 << bits) - 1);
			way   = search_markov_table(index, partial_vpn, 4);
			if(way != -1)
				hit_level = 4;
			else{
				bits  = ceil(log2(PT_S8_SETS/PT_S8_ASSOC));
				index = hash32(partial_vpn) & ((1 << bits) - 1);
				way   = search_markov_table(index, partial_vpn, 8);
				if(way != -1)
					hit_level = 8;
			}
		}
	}
	else
		hit_level = 1;

	if(way == -1){
		int fi_lim = -100;
		cache_line_pos = (current_vpn & 0x07);

		if(answer == -1 && ENABLE_FP == 1)
			ignore = 1;
		else{
			int free = 0;
			if(answer == -1)
				free = 1; 

			for(int fi=13; fi>=0; fi--){
				fi_lim = free_indexes[fi] + cache_line_pos;
				if((fi_lim >= 0) && (fi_lim <= 7)){
					prefetch_page(ip, addr, current_vpn + free_indexes[fi], FILL_L2, 0, free, free, free_indexes[fi], instr_id, type, iflag, 0, 0, 0);
					if(free == 0)
						free = 1;
				}
			}
		}

		int bits_1  = ceil(log2(PT_S1_SETS/PT_S1_ASSOC));
		int index_1 = hash32(partial_vpn) & ((1 << bits_1) - 1);
		int victim_1 = search_markov_table(index_1, 0, 1);
		if(victim_1 == -1){
			if(RP_MP == 0)
				victim_1 = lru_policy(index_1, 1);
			else if(RP_MP == 1)
				victim_1 = lfu_policy(index_1, 1);
			else if(RP_MP == 2)
				victim_1 = random_policy(1);
			else{
				cout << "Please use an already implemented replacement policy for the prediction table" << endl;
				assert(0);
			}
		}

		irip_s1[index_1][victim_1].vpn = partial_vpn;
		irip_s1[index_1][victim_1].timestamp = s_timer;
		irip_s1[index_1][victim_1].freq= 0;
		irip_s1[index_1][victim_1].successor_delta= 0;
	}
	else{
		uint64_t pf_page = 0;
		free_bit = 0; free_distance = 0;

		if(hit_level == 1){
			irip_s1[index][way].timestamp = s_timer;
			irip_s1[index][way].freq++;

			if(irip_s1[index][way].successor_delta != 0){
				if(answer == -1){
					cache_line_pos = (current_vpn & 0x07);
					if(irip_s1[index][way].successor_delta > 0){
						if(irip_s1[index][way].successor_delta + cache_line_pos < 8){
							free_bit = 1;
							free_distance = irip_s1[index][way].successor_delta + cache_line_pos;
						}
					}
					else{
						if(cache_line_pos - irip_s1[index][way].successor_delta < 8){
							free_bit = 1;
							free_distance = cache_line_pos - irip_s1[index][way].successor_delta;
						}
					}
				}

				pf_page = current_vpn + irip_s1[index][way].successor_delta;

				if(ENABLE_PREF_FP){
					clp = (pf_page & 0x07);
					free_prefetching(ip, current_vpn, clp, pf_page, free_indexes, instr_id, type, iflag);
				}

				prefetch_page(ip, current_vpn, pf_page, FILL_L2, 0, free_bit, free_bit, free_distance, instr_id, type, iflag, 0, 0, 1);
			}
		}
		else{
			int max_conf=0, max_conf_pos=0;
			if(hit_level == 2){
				irip_s2[index][way].timestamp = s_timer;
				irip_s2[index][way].freq++;
				max_conf = irip_s2[index][way].confidence[0];
				max_conf_pos = 0;
				for(int l=1; l<hit_level; ++l){
					if(irip_s2[index][way].confidence[l] > max_conf){
						max_conf = irip_s2[index][way].confidence[l];
						max_conf_pos = l;
					}
				}
			}
			else if(hit_level == 4){
				irip_s4[index][way].timestamp = s_timer;
				irip_s4[index][way].freq++;
				max_conf = irip_s4[index][way].confidence[0];
				max_conf_pos = 0;
				for(int l=1; l<hit_level; ++l){
					if(irip_s4[index][way].confidence[l] > max_conf){
						max_conf = irip_s4[index][way].confidence[l];
						max_conf_pos = l;
					}
				}
			}
			else if(hit_level == 8){
				irip_s8[index][way].timestamp = s_timer;
				irip_s8[index][way].freq++;
				max_conf = irip_s8[index][way].confidence[0];
				max_conf_pos = 0;
				for(int l=1; l<hit_level; ++l){
					if(irip_s8[index][way].confidence[l] > max_conf){
						max_conf = irip_s8[index][way].confidence[l];
						max_conf_pos = l;
					}
				}
			}
			else
				assert(0);


			for(int k=0; k<hit_level; ++k){
				if(hit_level == 2){
					if(irip_s2[index][way].successor_delta[k] != 0){
						free_bit = 0;
						free_distance = 0;
						cache_line_pos = (current_vpn & 0x07);
						if(irip_s2[index][way].successor_delta[k] > 0){
							if(irip_s2[index][way].successor_delta[k] + cache_line_pos < 8){
								free_bit = 1;
								free_distance = irip_s2[index][way].successor_delta[k] + cache_line_pos;
							}
						}
						else{
							if(cache_line_pos - irip_s2[index][way].successor_delta[k] < 8){
								free_bit = 1;
								free_distance = cache_line_pos - irip_s2[index][way].successor_delta[k];
							}
						}

						pf_page = current_vpn + irip_s2[index][way].successor_delta[k];

						if(ENABLE_PREF_FP && (max_conf_pos == k)){
							clp = (pf_page & 0x07);
							free_prefetching(ip, current_vpn, clp, pf_page, free_indexes, instr_id, type, iflag);
						}

						prefetch_page(ip, current_vpn, pf_page, FILL_L2, 0, free_bit, free_bit, free_distance, instr_id, type, iflag, 0, 0, 1);
					}
				}
				if(hit_level == 4){
					if(irip_s4[index][way].successor_delta[k] != 0){
						free_bit = 0; 
						free_distance = 0;
						cache_line_pos = (current_vpn & 0x07);
						if(irip_s4[index][way].successor_delta[k] > 0){ 
							if(irip_s4[index][way].successor_delta[k] + cache_line_pos < 8){
								free_bit = 1; 
								free_distance = irip_s4[index][way].successor_delta[k] + cache_line_pos;
							}
						}
						else{
							if(cache_line_pos - irip_s4[index][way].successor_delta[k] < 8){
								free_bit = 1; 
								free_distance = cache_line_pos - irip_s4[index][way].successor_delta[k];
							}
						}

						pf_page = current_vpn + irip_s4[index][way].successor_delta[k];

						if(ENABLE_PREF_FP && (max_conf_pos == k)){
							clp = (pf_page & 0x07);
							free_prefetching(ip, current_vpn, clp, pf_page, free_indexes, instr_id, type, iflag);
						}

						prefetch_page(ip, current_vpn, pf_page, FILL_L2, 0, free_bit, free_bit, free_distance, instr_id, type, iflag, 0, 0, 1);
					}
				}

				if(hit_level == 8){
					if(irip_s8[index][way].successor_delta[k] != 0){
						free_bit = 0;
						free_distance = 0;
						cache_line_pos = (current_vpn & 0x07);
						if(irip_s8[index][way].successor_delta[k] > 0){
							if(irip_s8[index][way].successor_delta[k] + cache_line_pos < 8){
								free_bit = 1;
								free_distance = irip_s8[index][way].successor_delta[k] + cache_line_pos;
							}
						}
						else{
							if(cache_line_pos - irip_s8[index][way].successor_delta[k] < 8){
								free_bit = 1;
								free_distance = cache_line_pos - irip_s8[index][way].successor_delta[k];
							}
						}

						pf_page = current_vpn + irip_s8[index][way].successor_delta[k];

						if(ENABLE_PREF_FP && (max_conf_pos == k)){
							clp = (pf_page & 0x07);
							free_prefetching(ip, current_vpn, clp, pf_page, free_indexes, instr_id, type, iflag);
						}

						prefetch_page(ip, current_vpn, pf_page, FILL_L2, 0, free_bit, free_bit, free_distance, instr_id, type, iflag, 0, 0, 1);
					}
				}
			}
		}
	}

	if(previous_vpn != 0){
		int bits_current, index_current, way_current;

		hit_level = -100;

		bits  = ceil(log2(PT_S1_SETS/PT_S1_ASSOC));
		index = hash32((previous_vpn & TAG_MASK)) & ((1 << bits) - 1);
		way   = search_markov_table(index, (previous_vpn & TAG_MASK), 1);

		if(way == -1){
			bits  = ceil(log2(PT_S2_SETS/PT_S2_ASSOC));
			index = hash32((previous_vpn & TAG_MASK)) & ((1 << bits) - 1);
			way   = search_markov_table(index, (previous_vpn & TAG_MASK), 2);
			if(way != -1){
				hit_level = 2;
			}
			else{
				bits  = ceil(log2(PT_S4_SETS/PT_S4_ASSOC));
				index = hash32((previous_vpn & TAG_MASK)) & ((1 << bits) - 1);
				way   = search_markov_table(index, (previous_vpn & TAG_MASK), 4);
				if(way != -1)
					hit_level = 4;
				else{
					bits  = ceil(log2(PT_S8_SETS/PT_S8_ASSOC));
					index = hash32((previous_vpn & TAG_MASK)) & ((1 << bits) - 1);
					way   = search_markov_table(index, (previous_vpn & TAG_MASK), 8);
					if(way != -1)
						hit_level = 8;
				}
			}
		}
		else
			hit_level = 1;

		int64_t delta = current_vpn - previous_vpn;
		int bypass = 0, placed = 0;

		if(hit_level == 1){
			if(irip_s1[index][way].successor_delta == 0)
				irip_s1[index][way].successor_delta = delta;
			else{
				if(delta != irip_s1[index][way].successor_delta){
					bits_current = ceil(log2(PT_S2_SETS/PT_S2_ASSOC));
					index_current = hash32((previous_vpn & TAG_MASK)) & ((1 << bits_current) - 1);
					way_current = search_markov_table(index_current, 0, 2);
					if(way_current == -1){
						if(RP_MP == 0)
							way_current = lru_policy(index_current, 2); 
						else if(RP_MP == 1)
							way_current = lfu_policy(index_current, 2); 
						else if(RP_MP == 2)
							way_current = random_policy(2);
						else{
							cout << "Please use an already implemented replacement policy for the prediction table" << endl;
							assert(0);
						}  
					}
					irip_s2[index_current][way_current].vpn = previous_vpn & TAG_MASK;
					irip_s2[index_current][way_current].successor_delta[0] = irip_s1[index][way].successor_delta;
					irip_s2[index_current][way_current].successor_delta[1] = delta;
					irip_s2[index_current][way_current].confidence[0] = 0;
					irip_s2[index_current][way_current].confidence[1] = 0;
					irip_s2[index_current][way_current].freq = irip_s1[index][way].freq;
					irip_s2[index_current][way_current].timestamp = irip_s1[index][way].timestamp; //s_timer;

					irip_s1[index][way].timestamp = 0;
					irip_s1[index][way].freq = 0;
					irip_s1[index][way].vpn = 0;
					irip_s1[index][way].successor_delta = 0;
				}
			}
		}
		else if(hit_level == 2){
			for(int j=0; j<2; ++j){
				if(irip_s2[index][way].successor_delta[j] == delta){
					bypass = 1;
					break;
				}
			}

			if(bypass == 0){
				for(int j=0; j<2; ++j){
					if(irip_s2[index][way].successor_delta[j] == 0){
						irip_s2[index][way].successor_delta[j] = delta;
						irip_s2[index][way].confidence[j] = 0;
						placed = 1;
						break;
					}
				}

				if(!placed){
					bits_current = ceil(log2(PT_S4_SETS/PT_S4_ASSOC));
					index_current = hash32((previous_vpn & TAG_MASK)) & ((1 << bits_current) - 1);
					way_current = search_markov_table(index_current, (previous_vpn & TAG_MASK), 4);

					if(way_current == -1){
						if(RP_MP == 0)
							way_current = lru_policy(index_current, 4);
						else if(RP_MP == 1)
							way_current = lfu_policy(index_current, 4);
						else if(RP_MP == 2)
							way_current = random_policy(4);
						else{
							cout << "Please use an already implemented replacement policy for the prediction table" << endl;
							assert(0);
						}
					}

					irip_s4[index_current][way_current].vpn = previous_vpn & TAG_MASK;
					irip_s4[index_current][way_current].successor_delta[0] = irip_s2[index][way].successor_delta[0];
					irip_s4[index_current][way_current].successor_delta[1] = irip_s2[index][way].successor_delta[1];
					irip_s4[index_current][way_current].successor_delta[2] = delta;
					irip_s4[index_current][way_current].successor_delta[3] = 0;
					irip_s4[index_current][way_current].confidence[0] = irip_s2[index][way].confidence[0];
					irip_s4[index_current][way_current].confidence[1] = irip_s2[index][way].confidence[1];
					irip_s4[index_current][way_current].confidence[2] = 0;
					irip_s4[index_current][way_current].confidence[3] = 0;
					irip_s4[index_current][way_current].freq = irip_s2[index][way].freq;
					irip_s4[index_current][way_current].timestamp = irip_s2[index][way].timestamp; //s_timer;

					irip_s2[index][way].timestamp = 0;
					irip_s2[index][way].freq = 0;
					irip_s2[index][way].vpn = 0;
					irip_s2[index][way].successor_delta[0] = 0;
					irip_s2[index][way].successor_delta[1] = 0;
					irip_s2[index][way].confidence[0] = 0;
					irip_s2[index][way].confidence[1] = 0;
				}
			}
		}
		else if(hit_level == 4){
			for(int j=0; j<4; ++j){
				if(irip_s4[index][way].successor_delta[j] == delta){
					bypass = 1;
					break;
				}
			}

			if(bypass == 0){ 
				for(int j=0; j<4; ++j){
					if(irip_s4[index][way].successor_delta[j] == 0){
						irip_s4[index][way].successor_delta[j] = delta;
						irip_s4[index][way].confidence[j] = 0;
						placed = 1;
						break;
					}
				}

				if(!placed){
					bits_current = ceil(log2(PT_S8_SETS/PT_S8_ASSOC));
					index_current = hash32((previous_vpn & TAG_MASK)) & ((1 << bits_current) - 1);
					way_current = search_markov_table(index_current, (previous_vpn & TAG_MASK), 8);

					if(way_current == -1){
						if(RP_MP == 0)
							way_current = lru_policy(index_current, 8);
						else if(RP_MP == 1)
							way_current = lfu_policy(index_current, 8);
						else if(RP_MP == 2)
							way_current = random_policy(8);
						else{
							cout << "Please use an already implemented replacement policy for the prediction table" << endl;
							assert(0);
						}
					}

					irip_s8[index_current][way_current].vpn = previous_vpn & TAG_MASK;
					irip_s8[index_current][way_current].successor_delta[0] = irip_s4[index][way].successor_delta[0];
					irip_s8[index_current][way_current].successor_delta[1] = irip_s4[index][way].successor_delta[1];
					irip_s8[index_current][way_current].successor_delta[2] = irip_s4[index][way].successor_delta[2];
					irip_s8[index_current][way_current].successor_delta[3] = irip_s4[index][way].successor_delta[3];
					irip_s8[index_current][way_current].successor_delta[4] = delta;
					irip_s8[index_current][way_current].successor_delta[5] = 0;
					irip_s8[index_current][way_current].successor_delta[6] = 0;
					irip_s8[index_current][way_current].successor_delta[7] = 0;
					irip_s8[index_current][way_current].confidence[0] = irip_s4[index][way].confidence[0];
					irip_s8[index_current][way_current].confidence[1] = irip_s4[index][way].confidence[1];
					irip_s8[index_current][way_current].confidence[2] = irip_s4[index][way].confidence[2];
					irip_s8[index_current][way_current].confidence[3] = irip_s4[index][way].confidence[3];
					irip_s8[index_current][way_current].confidence[4] = 0;
					irip_s8[index_current][way_current].confidence[5] = 0;
					irip_s8[index_current][way_current].confidence[6] = 0;
					irip_s8[index_current][way_current].confidence[7] = 0;
					irip_s8[index_current][way_current].freq = irip_s4[index][way].freq;
					irip_s8[index_current][way_current].timestamp = irip_s4[index][way].timestamp; //s_timer;

					irip_s4[index][way].timestamp = 0;
					irip_s4[index][way].freq = 0;
					irip_s4[index][way].vpn = 0;
					irip_s4[index][way].successor_delta[0] = 0;
					irip_s4[index][way].successor_delta[1] = 0;
					irip_s4[index][way].successor_delta[2] = 0;
					irip_s4[index][way].successor_delta[3] = 0;
					irip_s4[index][way].confidence[0] = 0;
					irip_s4[index][way].confidence[1] = 0;
					irip_s4[index][way].confidence[2] = 0;
					irip_s4[index][way].confidence[3] = 0;
				}
			}
		}
		else if(hit_level == 8){
			for(int j=0; j<8; ++j){
				if(irip_s8[index][way].successor_delta[j] == delta){
					bypass = 1;
					break;
				}
			}

			if(bypass == 0){ 
				for(int j=0; j<8; ++j){
					if(irip_s8[index][way].successor_delta[j] == 0){
						irip_s8[index][way].successor_delta[j] = delta;
						irip_s8[index][way].confidence[j] = 0;
						placed = 1;
						break;
					}
				}

				if(!placed){
					if(RP_MP == 1){
						int min_conf = irip_s8[index][way].confidence[0];
						int min_conf_pos = 0;
						for(int l=1; l<8; ++l){
							if(irip_s8[index][way].confidence[l] < min_conf){
								min_conf = irip_s8[index][way].confidence[l];
								min_conf_pos = l;
							}
						}

						irip_s8[index][way].successor_delta[min_conf_pos] = delta;
						irip_s8[index][way].confidence[min_conf_pos] = 0;
					}
					else{
						irip_s8[index][way].successor_delta[irip_s8[index][way].flag] = delta;
						irip_s8[index][way].flag = (irip_s8[index][way].flag + 1)%8;
					}
				}
			}
		}
	}

	if(decay_timer > RESET_FREQ){
		reset_frequency();
		decay_timer = 0;
	}

	previous_vpn = current_vpn;
}


void CACHE::stlb_prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr){

}

void CACHE::stlb_prefetcher_final_stats(uint64_t prefetches, uint64_t hits, uint64_t misses, uint64_t swap, uint64_t dupli, uint64_t free, uint64_t real, uint64_t * mmu_cache_demand_hits, uint64_t * mmu_cache_prefetch_hits, uint64_t * rfhits, uint64_t * free_hits, uint64_t mr[4][4], uint64_t stlb_misses[2])
{
	cout << endl << "*******************************************" << endl;
	cout << "CPU " << cpu << " STLB -- Morrigan final stats" << endl;
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
