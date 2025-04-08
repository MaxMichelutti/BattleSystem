#ifndef __RNG_MON_H__
#define __RNG_MON_H__

#include <random>

class RNG{
    private:
    std::mt19937 gen;
    public:
    RNG();
    ~RNG();
    // min and max are inclusive
    unsigned int random(unsigned int min, unsigned int max);
    static unsigned int getRandomInteger(unsigned int min, unsigned int max);
    static bool checkRandomEvent(unsigned int percentage);
    static bool coinFlip();
    static bool oneFourth();
    static bool oneEight();
};

static RNG rng;

#endif