#include "rng.h"

RNG::RNG() {
    std::random_device rd;
    gen = std::mt19937(rd());
}

RNG::~RNG() {
    
}

unsigned int RNG::random(unsigned int min, unsigned int max) {
    std::uniform_int_distribution<unsigned int> dist(min, max);
    return dist(gen);
}

unsigned int RNG::getRandomInteger(unsigned int min, unsigned int max) {
    return rng.random(min, max);
}

bool RNG::checkRandomEvent(unsigned int percentage) {
    if (percentage >= 100)
        return true;
    if (percentage <= 0)
        return false;
    return rng.random(1, 100) <= percentage;
}

bool RNG::coinFlip() {
    return rng.random(0, 1) == 1;
}

bool RNG::oneFourth() {
    return rng.random(1, 4) == 1;
}

bool RNG::oneEight() {
    return rng.random(1, 8) == 1;
}