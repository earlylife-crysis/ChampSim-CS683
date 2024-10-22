// Added by WrongAnswersOnly
// Implementation of Sampler for SBFP (Agile TLB, ISCA 2021)

#ifndef SAMPLER_H
#define SAMPLER_H

// Controls whether or nor SBFP is used
#define SBFP_ENABLE

#include <cstdint>

#define SAMPLER_SIZE 64

class sampler
{
    private:
    uint8_t* valid;
    uint8_t* fifo;
    int8_t* free_pf_dist;
    uint64_t* vpn;

    public:
    void add_entry(uint64_t, int8_t);
    int8_t check_hit(uint64_t);
    void print_contents();

    // Defining the constructor
    sampler()
    {
        valid = new uint8_t[SAMPLER_SIZE]();
        fifo = new uint8_t[SAMPLER_SIZE]();
        free_pf_dist = new int8_t[SAMPLER_SIZE]();
        vpn = new uint64_t[SAMPLER_SIZE]();
    }
};

#endif