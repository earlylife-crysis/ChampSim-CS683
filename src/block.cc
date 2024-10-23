#include "block.h"

#include <vector>     // for 2D vector 
#include <algorithm>  // for sort() 
#include <random>

void PACKET_QUEUE::print_pq()
{
    if ((head == tail) && occupancy == 0)
        cout << "Empty queue, with size: " << SIZE << endl;
    else{
        for(int i = 0; i < SIZE; i++)
            cout << hex << entry[i].address << endl;  
    }   
}

uint64_t PACKET_QUEUE::return_event_cycle(int cell)
{
    return entry[cell].event_cycle;
}

pair<int, uint64_t> PACKET_QUEUE::check_queue_vpn(uint64_t vpn)
{
    /* return_pair.first: position_in_pq or -1
     * return_pair.second: event_cycle or 0
     */
    pair <int, int> return_pair;

    if ((head == tail) && occupancy == 0){ 
        return_pair = make_pair(-1,0);
        return return_pair;
    }   

    for(int i = 0; i < SIZE; i++){
        if (entry[i].address == vpn){
            return_pair = make_pair(i, entry[i].event_cycle);
            return return_pair;
        }   
    }   

    return_pair = make_pair(-1,0);
    return return_pair;
}




int PACKET_QUEUE::check_queue(PACKET *packet)
{
    if ((head == tail) && occupancy == 0)
        return -1;

    if (head < tail) {
        for (uint32_t i=head; i<tail; i++) {
            if (NAME == "L1D_WQ") {
                if (entry[i].full_addr == packet->full_addr) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
            else {
                if (entry[i].address == packet->address) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
        }
    }
    else {
        for (uint32_t i=head; i<SIZE; i++) {
            if (NAME == "L1D_WQ") {
                if (entry[i].full_addr == packet->full_addr) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
            else {
                if (entry[i].address == packet->address) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
        }
        for (uint32_t i=0; i<tail; i++) {
            if (NAME == "L1D_WQ") {
                if (entry[i].full_addr == packet->full_addr) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
            else {
                if (entry[i].address == packet->address) {
                    DP (if (warmup_complete[packet->cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id << " same address: " << hex << packet->address;
                    cout << " full_addr: " << packet->full_addr << dec << " by instr_id: " << entry[i].instr_id << " index: " << i;
                    cout << " cycle " << packet->event_cycle << endl; });
                    return i;
                }
            }
        }
    }

    return -1;
}

void PACKET_QUEUE::add_queue(PACKET *packet)
{
#ifdef SANITY_CHECK
    if (occupancy && (head == tail))
        assert(0);
#endif

    // add entry
    entry[tail] = *packet;

    DP ( if (warmup_complete[packet->cpu]) {
    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id;
    cout << " address: " << hex << entry[tail].address << " full_addr: " << entry[tail].full_addr << dec;
    cout << " head: " << head << " tail: " << tail << " occupancy: " << occupancy << " event_cycle: " << entry[tail].event_cycle << endl; });

    occupancy++;
    tail++;
    if (tail >= SIZE)
        tail = 0;
}

void PACKET_QUEUE::remove_queue_prefetch(PACKET *packet, int pos){
    PACKET empty_packet;
    *packet = empty_packet;
    if (head == tail){
        head = pos;
        tail = pos;
    }
    head++;
    occupancy--;
    if (head >= SIZE)
        head = 0;
}

bool sortcol1(const vector<int>& v1, const vector<int>& v2 ){
    return (v1[1] < v2[1]);
}

uint64_t PACKET_QUEUE::remove_queue_lru(){
    uint64_t min_cycles = 0xFFFFFFFFFFFFFFFF;
    int evicted_cell = -1;
    PACKET empty_packet;
    int flag = 0;


    for(int i = 0; i < SIZE; i++){
        //cout << "ec: " << dec << entry[i].event_cycle << ", conf: " << dec << entry[i].conf << endl;
        if (entry[i].event_cycle < min_cycles){
            min_cycles = entry[i].event_cycle;
            evicted_cell = i;
        }
    }
    uint64_t toreturn = 0;
    if(evicted_cell != -1){
        toreturn = entry[evicted_cell].address;
        entry[evicted_cell] = empty_packet;
        occupancy--;
    }
    return toreturn;
}




void PACKET_QUEUE::remove_queue(PACKET *packet)
{
#ifdef SANITY_CHECK
    if ((occupancy == 0) && (head == tail))
        assert(0);
#endif

    DP ( if (warmup_complete[packet->cpu]) {
    cout << "[" << NAME << "] " << __func__ << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id;
    cout << " address: " << hex << packet->address << " full_addr: " << packet->full_addr << dec << " fill_level: " << packet->fill_level;
    cout << " head: " << head << " tail: " << tail << " occupancy: " << occupancy << " event_cycle: " << packet->event_cycle << endl; });

    // reset entry
    PACKET empty_packet;
    *packet = empty_packet;

    occupancy--;
    head++;
    if (head >= SIZE)
        head = 0;
}


int PACKET_QUEUE::update_tail(){
	int i;
    for (i=0; i<SIZE; i++){
        if (entry[i].address == 0){
            tail = i;
            break;
        }
    }
    return i;
}
