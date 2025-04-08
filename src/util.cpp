#include "util.h"

unsigned int max(unsigned int a, unsigned int b) {
    return a > b ? a : b;
}

unsigned int min(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}

unsigned int stringToInteger(std::string str) {
    std::stringstream ss(str);
    unsigned int result;
    ss >> result;
    return result;
}

std::string oneDecimalIntegerToString(unsigned int integer){
    return std::to_string(integer/10) + "." + std::to_string(integer%10);
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

double baseDamage(unsigned int level, unsigned int base_power, unsigned int attack, unsigned int defense) {
    return ((((2 * level / 5.0 + 2) * base_power * attack / defense) / 50) + 2);
}