// Added by WrongAnswersOnly
// Implementation of Sampler for SBFP (Agile TLB, ISCA 2021)

#include "sampler.h"
#include <iostream>
#include <cassert>

void sampler::add_entry(uint64_t vpn_entered, int8_t free_pf_dist_entered)
{
    // First check if an entry is available in the sampler
    int sampler_entry = SAMPLER_SIZE;
    for(int i = 0; i < SAMPLER_SIZE; i++)
    {
        if(valid[i] == 0)
        {
            sampler_entry = i;
            // Increment FIFO counters
            for(int j = 0; j < i; j++)
            {
                fifo[j]++;
            }
            break;
        }
    }

    // Evict FI entry if no entry available, and update FIFO values
    if(sampler_entry == SAMPLER_SIZE)
    {
        for(int i = 0; i < SAMPLER_SIZE; i++)
        {
            fifo[i]++;
            if(fifo[i] == SAMPLER_SIZE)
            {
                valid[i] = 0;
                sampler_entry = i;
                fifo[i] = 0;
            }
        }
    }

    // Invalid case
    if(sampler_entry == SAMPLER_SIZE)
    {
        assert(0);
    }

    // Insert recieved arguments into the chosen entry
    valid[sampler_entry] = 1;
    vpn[sampler_entry] = vpn_entered;
    free_pf_dist[sampler_entry] = free_pf_dist_entered;
}

int8_t sampler::check_hit(uint64_t vpn_check)
{
    // Check if requested entry is present in sampler
    for(int i = 0; i < SAMPLER_SIZE; i++)
    {
        if(valid[i] != 0 && vpn[i] == vpn_check)
        {
            return free_pf_dist[i];
        }
    }

    // Return 0 if entry not present in sampler
    return(0);
}

void sampler::print_contents()
{
    for(int i = 0; i < SAMPLER_SIZE; i++)
    {
        std::cout << vpn[i] << " " << (int) free_pf_dist[i] << " " << (uint) fifo[i] << "\n";
    }
}