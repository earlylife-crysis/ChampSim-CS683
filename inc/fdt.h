// Added by WrongAnswersOnly
// Implementation of FDT for SBFP (Agile TLB, ISCA 2021)

#ifndef FDT_H
#define FDT_H

#include <cstdint>

#define FDT_SIZE 14
#define COUNTER_SATURATION_POINT 1024
#define PREFETCH_THRESHOLD 100

class fdt
{
    private:
    uint16_t* counters;

    public:
    void update_fdt(int8_t);
    bool insert_sampler(int8_t);  // Returns true if free prefetch should be inserted into sampler
    void print_fdt();
    fdt()
    {
        counters = new uint16_t[FDT_SIZE]();
    }
};

#endif