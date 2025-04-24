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

std::string internalToDisplay(std::string str){
    std::string result = str;
    bool is_first_letter = true;
    for (size_t i = 0; i < result.length(); ++i) {
        if(isalpha(result[i])){
            if(is_first_letter){
                result[i] = toupper(result[i]);
                is_first_letter = false;
            }else{
                result[i] = tolower(result[i]);
            }
        }
        if (result[i] == '_') {
            result[i] = ' ';
            is_first_letter = true;
        }
    }
    return result;
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

int getHour(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    return std::localtime(&now_time)->tm_hour;
}

bool isDay(){
    //check if current system time is between 6:00 AM and 6:00 PM
    int hour = getHour();
    return (hour >= 6 && hour < 18);
}
bool isNight(){
    //check if current system time is between 6:00 PM and 6:00 AM
    int hour = getHour();
    return (hour < 6 || hour >= 18);
}