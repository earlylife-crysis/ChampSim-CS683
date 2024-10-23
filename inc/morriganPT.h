#define PT_S1_SETS 128
#define PT_S1_ASSOC 32

#define PT_S2_SETS 128
#define PT_S2_ASSOC 32

#define PT_S4_SETS 128
#define PT_S4_ASSOC 32

#define PT_S8_SETS 64
#define PT_S8_ASSOC 16

#define TAG_BITS 16
#define TAG_MASK ((1 << TAG_BITS) - 1)

int search_markov_table(int index, uint64_t current_vpn, int id);
int lru_policy(int index, int id);
int lfu_policy(int index, int id);
int random_policy(int id);

uint32_t hash32(uint32_t a);

void reset_frequency(), show_s1(), show_s2(), show_s4(), show_s8();

bool sortcol(const vector<int>& v1, const vector<int>& v2);

class MARKOV_TABLE{
	public:
		uint64_t vpn;
		uint64_t timestamp; // for LRU policy
		int freq; // for LFU policy
		MARKOV_TABLE(){
			vpn = 0;
			timestamp = 0;
			freq = 0;
		}   
};

class MARKOV_S1 : public MARKOV_TABLE{
	public:
		int64_t successor_delta;
		MARKOV_S1(){
			successor_delta = 0;
		}
};

class MARKOV_S2 : public MARKOV_TABLE{
	public:
		int64_t successor_delta[2];
		int64_t confidence[2];
		MARKOV_S2(){
			for(int i=0; i<2; ++i){
				successor_delta[i] = 0;
				confidence[i] = 0;
			}
		}
};

class MARKOV_S4 : public MARKOV_TABLE{
	public:
		int64_t successor_delta[4];
		int64_t confidence[4];
		MARKOV_S4(){
			for(int i=0; i<4; ++i){
				successor_delta[i] = 0;
				confidence[i] = 0;
			}
		}
};

class MARKOV_S8 : public MARKOV_TABLE{
	public:
		int64_t successor_delta[8];
		int64_t confidence[8];
		int flag; // for FIFO policy
		MARKOV_S8(){
			for(int i=0; i<8; ++i){
				successor_delta[i] = 0;
				confidence[i] = 0;
			}
			flag = 0;
		}
};

