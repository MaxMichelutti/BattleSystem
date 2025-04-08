#include "expcurve.h"

unsigned int getTotalExpForNextLevel(unsigned int level,ExpCurve curve){
    if(level == MAX_LEVEL)
        return 0;
    unsigned int next_level = level + 1;
    unsigned int type_index = static_cast<unsigned int>(curve);
    return exp_table[type_index][next_level];
}

unsigned int getLevelFromExp(unsigned long exp,ExpCurve curve){
    unsigned int type_index = static_cast<unsigned int>(curve);
    for(unsigned int i = 1; i <= MAX_LEVEL; i++){
        if(exp_table[type_index][i] > exp)
            return i - 1;
    }
    return MAX_LEVEL;
}

unsigned long getExpForLevel(unsigned int level,ExpCurve curve){
    unsigned int type_index = static_cast<unsigned int>(curve);
    return exp_table[type_index][level];
}

std::string expCurveToString(ExpCurve curve){
    switch(curve){
        case ERRATIC:
            return "ERRATIC";
        case FAST:
            return "FAST";
        case MEDIUM_FAST:
            return "MEDIUM_FAST";
        case MEDIUM_SLOW:
            return "MEDIUM_SLOW";
        case SLOW:
            return "SLOW";
        case FLUCTUATING:
            return "FLUCTUATING";
        default:
            return "MEDIUM_FAST";
    }
}

ExpCurve stringToExpCurve(std::string curve){
    if(curve == "ERRATIC")
        return ERRATIC;
    if(curve == "FAST")
        return FAST;
    if(curve == "MEDIUM_FAST")
        return MEDIUM_FAST;
    if(curve == "MEDIUM_SLOW")
        return MEDIUM_SLOW;
    if(curve == "SLOW")
        return SLOW;
    if(curve == "FLUCTUATING")
        return FLUCTUATING;
    return ERRATIC;
}