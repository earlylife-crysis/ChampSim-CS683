// Added by WrongAnswersOnly (WAO)
// Implementation of an FPQ class for Agile STLB Prefetching

#include "fpq.h"
#include <iostream>
#include <cassert>

void fpq::add_fpq(uint64_t vpn_insert)
{
    // First check if an entry corresponding to the vpn is available
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(vpn[i] == vpn_insert && valid[i] == 1)
        {
            // Update the fifo values for other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(fifo[j] < fifo[i] && valid[j] == 1)
                {
                    fifo[j]++;
                }
            }
            fifo[i] = 0;  // Set fifo to 0 for chosen entry
            return;
        }
    }

    // Check if an empty entry is available
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(valid[i] == 0)
        {
            valid[i] = 1;
            fifo[i] = 0;
            vpn[i] = vpn_insert;

            // Update fifo values for other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(i != j && valid[j] == 1)
                {
                    fifo[j]++;
                }
            }
            return;
        }
    }

    // If no entry available, evict the FI entry
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(fifo[i] == FPQ_SIZE - 1)
        {
            fifo[i] = 0;
            vpn[i] = vpn_insert;

            // Update fifo values for other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(i != j)
                {
                    fifo[j]++;
                }
            }
            return;
        }
    }

    // Should never reach here
    assert(0);
}

bool fpq::hit_fpq(uint64_t vpn_check)
{
    // Check if VPN is present in the FPQ
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(valid[i] == 1 && vpn[i] == vpn_check)
        {
            return true;
        }
    }
    return false;
}

void fpq::print_fpq()
{
    // Print the contents of the FPQ (for debug)
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        std::cout << (uint)valid[i] << " " << (uint)fifo[i] << " " << vpn[i] << std::endl;
    }
}