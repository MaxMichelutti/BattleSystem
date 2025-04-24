#ifndef __UTIL_MON_H__
#define __UTIL_MON_H__
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>

template <typename T> T max_template(T a, T b) {
    return a > b ? a : b;
}

template <typename T> T min_template(T a, T b) {
    return a < b ? a : b;
}

unsigned int max(unsigned int a, unsigned int b);
unsigned int min(unsigned int a, unsigned int b);

unsigned int stringToInteger(std::string str);

std::string oneDecimalIntegerToString(unsigned int integer);
std::string internalToDisplay(std::string str);
bool is_number(const std::string& s);

double baseDamage(unsigned int level, unsigned int base_power, unsigned int attack, unsigned int defense);

bool isDay();
bool isNight();

#endif