// Added by WrongAnswersOnly
// Implementation of FDT for SBFP (Agile TLB, ISCA 2021)

#include "fdt.h"
#include <iostream>
#include <cassert>

void fdt::update_fdt(int8_t free_pf_dist)
{
    // Increment appropriate counter, ensuring no out of bound behaviour
    int counter_index = FDT_SIZE;
    if(free_pf_dist < 0)
    {
        counter_index = free_pf_dist + (FDT_SIZE / 2);
    }
    else if(free_pf_dist > 0)
    {
        counter_index = free_pf_dist + (FDT_SIZE / 2) - 1;
    }

    if(counter_index == FDT_SIZE)
    {
        assert(0);  // Impossible state (can occur if free_pf_distance is 0)
    }

    counters[counter_index]++;

    // Check if the counter is saturated after updation, and right shift by one bit
    if(counters[counter_index] == COUNTER_SATURATION_POINT)
    {
        for(int i = 0; i < FDT_SIZE; i++)
        {
            counters[i] = counters[i] >> 1;
        }
    }
}

bool fdt::insert_sampler(int8_t free_pf_dist)
{
    // Query appropriate counter, ensuring no out of bound behaviour
    int counter_index = FDT_SIZE;
    if(free_pf_dist < 0)
    {
        counter_index = free_pf_dist + (FDT_SIZE / 2);
    }
    else if(free_pf_dist > 0)
    {
        counter_index = free_pf_dist + (FDT_SIZE / 2) - 1;
    }

    if(counter_index == FDT_SIZE)
    {
        assert(0);  // Impossible state (can occur if free_pf_distance is 0)
    }

    if(counters[counter_index] < PREFETCH_THRESHOLD)
    {
        return true;
    }
    else
    {
        return false;
    }

    // Impossible state
    assert(0);
}

void fdt::print_fdt()
{
    for(int i = 0; i < FDT_SIZE; i++)
    {
        std::cout << counters[i] << " ";
    }
}