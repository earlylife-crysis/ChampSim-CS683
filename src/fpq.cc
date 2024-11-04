// Added by WrongAnswersOnly (WAO)
// Implementation of an FPQ class for Agile STLB Prefetching

#include "fpq.h"
#include <iostream>
#include <cassert>

void fpq::add_fpq(uint64_t vpn_insert)
{
    // First check if entry already exists
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(valid[i] == 1 && vpn[i] == vpn_insert)
        {
            lru[i] = 0;
            // Update LRU for all other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(j != i && valid[j] == 1)
                {
                    lru[j]++;
                }
            }
            return;
        }
    }

    // If entry does not exist, find if an empty entry is available
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(valid[i] == 0)
        {
            vpn[i] = vpn_insert;
            valid[i] = 1;
            lru[i] = 0;
            // Update LRU for all other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(j != i && valid[j] == 1)
                {
                    lru[j]++;
                }
            }
            return;
        }
    }

    // If no empty entry is available, find the LRU entry
    for(int i = 0; i < FPQ_SIZE; i++)
    {
        if(lru[i] == FPQ_SIZE - 1)
        {
            vpn[i] = vpn_insert;
            valid[i] = 1;
            lru[i] = 0;
            // Update LRU for all other valid entries
            for(int j = 0; j < FPQ_SIZE; j++)
            {
                if(j != i && valid[j] == 1)
                {
                    lru[j]++;
                }
            }
            return;
        }
    }
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
        std::cout << (uint)valid[i] << " " << (uint)lru[i] << " " << vpn[i] << std::endl;
    }
}