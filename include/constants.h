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
const unsigned int PURSUIT_ID = 388;
const unsigned int SKETCH_ID = 455;
const unsigned int DRAGON_ASCENT_ID = 532;
const unsigned int DOOM_DESIRE_ID = 535;
const unsigned int FUSION_BOLT_ID = 591;
const unsigned int FUSION_FLARE_ID = 589;
const unsigned int SECRET_SWORD_ID = 596;
const unsigned int RELIC_SONG_ID = 597;

const unsigned int MAX_UNSIGNED = 4294967295;

// stat modifiers
const int MAX_MODIFIER = 6;
const int MIN_MODIFIER = -6;

// FILES
const std::string ATTACK_FILE = "data/attacks.data";
const std::string SPECIES_FILE = "data/species.data";
const std::string FORMS_FILE = "data/forms.data";
const std::string ABILITIES_FILE = "data/abilities.data";
const std::string ITEMS_FILE = "data/items.data";

// RETURN ERRORS
const int SUCCESS = 0;
const int FILE_ERROR = 1;
const int DATA_ERROR = 2;
const int NO_ALIVE_ERROR = 3;

#endif