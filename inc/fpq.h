// Added by WrongAnswersOnly (WAO)
// Implementation of an FPQ class for Agile STLB Prefetching

#ifndef FPQ_H
#define FPQ_H

#include <cstdint>

// Define FPQ size
#define FPQ_SIZE 64

class fpq
{
    private:
    uint8_t* lru;
    uint64_t* vpn;
    uint8_t* valid;

    public:
    void add_fpq(uint64_t);
    bool hit_fpq(uint64_t);  // Returns true if hit in FPQ
    void print_fpq();
    fpq()
    {
        lru = new uint8_t[FPQ_SIZE]();
        vpn = new uint64_t[FPQ_SIZE]();
        valid = new uint8_t[FPQ_SIZE]();
    }
};

#endif