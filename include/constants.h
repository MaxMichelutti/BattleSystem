#ifndef __CONST_MON_H__
#define __CONST_MON_H__

// #define DEBUG

#include <string>

// CONSTANT VALUES
const unsigned int MAX_TOTAL_EVS = 510;
const unsigned int MAX_EVS = 252;
const unsigned int MAX_IVS = 31;
const unsigned int MAX_LEVEL = 100;
const unsigned int ALWAYS_HITS = 200;
const unsigned int MAX_FRIENDSHIP = 255;

// important attack IDs
const unsigned int STRUGGLE_ID = 18;
const unsigned int MIMIC_ID = 168;
const unsigned int ROLLOUT_ID = 127;
const unsigned int TRANSFORM_ID = 192;
const unsigned int FUTURE_SIGHT_ID = 235;
const unsigned int MINIMIZE_ID = 261;
const unsigned int DEFENSE_CURL_ID = 126;
const unsigned int FOCUS_PUNCH_ID = 330;

// stat modifiers
const int MAX_MODIFIER = 6;
const int MIN_MODIFIER = -6;

// FILES
const std::string ATTACK_FILE = "data/attacks.data";
const std::string SPECIES_FILE = "data/species.data";
const std::string ABILITIES_FILE = "data/abilities.data";

// RETURN ERRORS
const int SUCCESS = 0;
const int FILE_ERROR = 1;
const int DATA_ERROR = 2;

#endif