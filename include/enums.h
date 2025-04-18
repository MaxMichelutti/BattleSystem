#ifndef __INCLUDE_ENUMS_H_____
#define __INCLUDE_ENUMS_H_____

enum Weather{
    CLEAR,
    RAIN,
    SUN,
    SANDSTORM,
    HAIL,
    SNOWSTORM
};

enum Terrain{
    NO_TERRAIN,
    GRASSY_FIELD,
    MISTY_FIELD,
    ELECTRIC_FIELD,
    PSYCHIC_FIELD
};

enum FormKind{
    NO_FORM_KIND,
    ALTERNATE_FORM,
    MEGA_EVOLUTION,
    ALOLAN_FORM,
    GALARIAN_FORM,
    PALDEAN_FORM,
    HISUIAN_FORM
};

enum FieldEffect{
    NO_FIELD_EFFECT,
    // FIRE_SPIN,
    TAILWIND,
    SAFEGUARD,
    TOXIC_SPIKES,
    BAD_TOXIC_SPIKES,
    LIGHT_SCREEN,
    REFLECT,
    // SAND_TOMB,
    GRAVITY,
    WONDER_ROOM,
    STEALTH_ROCKS,
    SPIKES,
    SPIKES_2,
    SPIKES_3,
    MIST,
    AURORA_VEIL,
    TRICK_ROOM
    // AURORA_VEIL,
    // MAGIC_COAT,
    // TRICK_ROOM,
};
enum BattleActionActor{
    PLAYER,
    OPPONENT
};
enum BattleActionType{
    ATTACK,
    SWITCH,
    PETAL_DANCE,
    RAGING_FURY,
    OUTRAGE,
    THRASH,
    SOLAR_BEAM,
    FLY,
    ROLLOUT,
    DIG,
    DIVE,
    UPROAR,
    RECHARGE_TURN,
    SKY_ATTACK,
    USE_ITEM
};
enum PermanentStatusCondition{
    NO_PERMANENT_CONDITION,
    BURN,
    FREEZE,
    PARALYSIS,
    POISONED,
    BAD_POISON,
    SLEEP_1,
    SLEEP_2,
    SLEEP_3,
    SLEEP_4,
    KO
};
enum VolatileStatusCondition{
    CONFUSION,
    FLINCH,
    INFATUATION,
    LEECH_SEED,
    PERISH_SONG,
    CHARGING_SOLARBEAM,
    CHARGING_SKYATTACK,
    RECHARGING,
    GROUNDED,
    PETAL_DANCING,
    RAGING_FURING,
    OUTRAGING,
    THRASHING,
    PROTECT,
    LASER_FOCUS,
    FOCUS_ENERGY,
    FLYING_HIGH,
    WRAP,
    BIND,
    FIRESPIN,
    SANDTOMB,
    ROLLINGOUT,
    UNDERGROUND,
    UNDERWATER,
    ENCORE,
    IMPRISON,
    MEAN_LOOK,
    QUICK_GUARD,
    TAUNTED,
    SMACKED_DOWN,
    DROWSY,
    CURSED,
    MAGNET_RISE,
    UPROARING,
    AQUA_RING,
    WHIRLPOOL,
    DESTINY_BOND,
    CHARGED,
    CHARGED_2, // used to apply charged effect that specific turn only
    ENDURE,
    FOCUSED,
    INGRAINED,
    BLOCKED,
    FLASH_FIRED,
    UNNERVED,
    MOVING_FIRST,
    INCREASED_CRIT,
    INCREASED_ACCURACY,
    SWITCHING_OUT,
    JUST_EATEN_BERRY
};

enum Gender{
    MALE,
    FEMALE,
    GENDERLESS
};

enum GenderRule{
    NO_GENDER,
    HALF_MALE_HALF_FEMALE,
    FEMALE_ONE_EIGHT,
    MALE_ONE_EIGHT,
    ALWAYS_MALE,
    ALWAYS_FEMALE,
    MALE_ONE_FOURTH,
    FEMALE_ONE_FOURTH
};

enum Nature{
    HARDY,
    LONELY,
    BRAVE,
    ADAMANT,
    NAUGHTY,
    BOLD,
    DOCILE,
    RELAXED,
    IMPISH,
    LAX,
    TIMID,
    HASTY,
    SERIOUS,
    JOLLY,
    NAIVE,
    MODEST,
    MILD,
    QUIET,
    BASHFUL,
    RASH,
    CALM,
    GENTLE,
    SASSY,
    CAREFUL,
    QUIRKY
};

enum Type{
    NO_TYPE,
    NORMAL,
    FIRE,
    WATER,
    ELECTRIC,
    GRASS,
    ICE,
    FIGHTING,
    POISON,
    GROUND,
    FLYING,
    PSYCHIC,
    BUG,
    ROCK,
    GHOST,
    DRAGON,
    DARK,
    STEEL,
    FAIRY
};
enum AttackType{
    PHYSICAL,
    SPECIAL,
    STATUS
};
enum AttackTarget{
    TARGET_SELF,
    TARGET_OPPONENT
};
enum ExpCurve{
    ERRATIC,
    FAST,
    MEDIUM_FAST,
    MEDIUM_SLOW,
    SLOW,
    FLUCTUATING
};
enum EvolutionMethod{
    LEVEL,
    USE_EVO_ITEM,
    TRADE,
    LEVEL_NIGHT,
    FRIENDSHIP
};

enum Ability{
    NO_ABILITY,
    OVERGROW,
    CHLOROPHYL,
    BLAZE,
    SOLAR_POWER,
    TORRENT,
    SWIFT_SWIM,
    INSOMNIA,
    RAIN_DISH,
    SHED_SKIN,
    SHIELD_DUST,
    RUN_AWAY,
    COMPOUND_EYES,
    TINTED_LENS,
    SNIPER,
    SWARM,
    KEEN_EYE,
    TANGLED_FEET,
    BIG_PECKS,
    GUTS,
    HUSTLE,
    INTIMIDATE,
    UNNERVE,//only works with items
    STATIC,
    LIGHTNING_ROD,
    SAND_VEIL,
    SAND_RUSH,
    POISON_POINT,
    RIVALRY,
    SHEER_FORCE,
    CUTE_CHARM,
    MAGIC_GUARD,
    FRIEND_GUARD,//only works in doubles
    UNAWARE,
    FLASH_FIRE,
    DROUGHT,
    COMPETITIVE,
    FRISK,
    INFILTRATOR,
    INNER_FOCUS,
    STENCH,//no battle effect
    EFFECT_SPORE,
    DAMP,
    DRY_SKIN,
    WONDER_SKIN,
    ARENA_TRAP,
    IMPOSTOR,
    LIMBER,
    SAND_FORCE,
    PICKUP,
    TECHNICIAN,
    CLOUD_NINE,
    VITAL_SPIRIT,
    ANGER_POINT,
    DEFIANT,
    JUSTIFIED,
    WATER_ABSORB,
    SYNCHRONIZE,
    NO_GUARD,
    STEADFAST,
    GLUTTONY,//only works with items (berries)
    LIQUID_OOZE,
    CLEAR_BODY,
    STURDY,
    ROCK_HEAD,
    FLAME_BODY,
    OWN_TEMPO,
    OBLIVIOUS,
    REGENERATOR,
    MAGNET_PULL,
    ANALYTIC,
    EARLY_BIRD,
    THICK_FAT,
    HYDRATION,
    ICE_BODY,
    STICKY_HOLD,
    POISON_TOUCH,
    SHELL_ARMOR,
    SKILL_LINK,
    OVERCOAT,
    LEVITATE,
    CURSED_BODY,
    WEAK_ARMOR,
    FOREWARN,
    HYPER_CUTTER,
    SOUNDPROOF,
    AFTERMATH,
    PLUS,
    MINUS,
    HARVEST,
    BATTLE_ARMOR,
    RECKLESS,
    UNBURDEN,
    IRON_FIST,
    NEUTRALIZING_GAS,
    NATURAL_CURE,
    SERENE_GRACE,
    HEALER, //only works in doubles
    LEAF_GUARD,
    SCRAPPY,
    WATER_VEIL,
    ILLUMINATE,
    FILTER,
    MOLD_BREAKER,
    MOXIE,
    RATTLED,
    ADAPTABILITY,
    ANTICIPATION,
    VOLT_ABSORB,
    QUICK_FEET,
    TRACE,
    DOWNLOAD,
    PRESSURE,
    IMMUNITY,
    SNOW_CLOAK,
    MULTISCALE,
    MARVEL_SCALE,
    TOUGH_CLAWS,
    MEGA_LAUNCHER,
    SURGE_SURFER,
    SLUSH_RUSH,
    SNOW_WARNING,
    TANGLING_HAIR,
    FUR_COAT,
    GALVANIZE,
    PASTEL_VEIL,
    QUICK_DRAW,
    POWER_OF_ALCHEMY,
    SHADOW_TAG,
    MISTY_SURGE,
    PARENTAL_BOND,
    SCREEN_CLEANER,
    AERILATE,
    CUD_CHEW,
    BERSERK
};

enum ItemCategory{
    NO_CATEGORY,
    KEY_ITEM,
    BERRY,
    BALL,
    MEDICINE,
    EVOLUTION_ITEM,
    TM,
    HELD_ITEM,
    GENERAL_ITEM,
    MEGA_STONE,
    FOSSIL
};

enum ItemType{
    NO_ITEM_TYPE,
    POTION,
    SUPER_POTION,
    HYPER_POTION,
    MAX_POTION,
    FULL_RESTORE,
    ANTIDOTE,
    AWAKENING,
    PARALYZE_HEAL,
    BURN_HEAL,
    ICE_HEAL,
    FULL_HEAL,
    ORAN_BERRY,
    AGUAV_BERRY,
    APICOT_BERRY,
    CUSTAP_BERRY,
    FIGY_BERRY,
    GANION_BERRY,
    IAPAPA_BERRY,
    LANSAT_BERRY,
    LIECHI_BERRY,
    MAGO_BERRY,
    MICLE_BERRY,
    PETAYA_BERRY,
    SALAC_BERRY,
    SITRUS_BERRY,
    STARF_BERRY,
    WIKI_BERRY,
    ASPEAR_BERRY,
    CHERY_BERRY,
    CHESTO_BERRY,
    PECHA_BERRY,
    RAWST_BERRY,
    PERSIM_BERRY,
    LEPPA_BERRY,
    ENIGMA_BERRY,
    JABOCA_BERRY,
    ROWAP_BERRY,
    KEE_BERRY,
    MARANGA_BERRY,
    BABIRI_BERRY,
    CHARTI_BERRY,
    CHILAN_BERRY,
    CHOPLE_BERRY,
    COBA_BERRY,
    COLBUR_BERRY,
    HABAN_BERRY,
    KASIB_BERRY,
    KEBIA_BERRY,
    OCCA_BERRY,
    PASSHO_BERRY,
    PAYAPA_BERRY,
    RINDO_BERRY,
    ROSELI_BERRY,
    SHUCA_BERRY,
    TANGA_BERRY,
    WACAN_BERRY,
    YACHE_BERRY,
    GREPA_BERRY,
    HONDEW_BERRY,
    KELPSY_BERRY,
    POMEG_BERRY,
    QUALOT_BERRY,
    TAMATO_BERRY,
    LUM_BERRY,
    VENUSAURITE,
    CHARIZARDITE_X,
    CHARIZARDITE_Y,
    BLASTOISINITE,
    BEEDRILLITE,
    PIDGEOTITE,
    ALAKAZITE,
    SLOWBRONITE,
    GENGARITE,
    KANGASKHANTITE,
    PINSIRITE,
    GYARADOSITE,
    AERODACTYLITE,
    MEWTWONITE_X,
    MEWTWONITE_Y,
    AUSPICIOUS_ARMOR,
    BLACK_AUGURITE,
    CHIPPED_POT,
    CRACKED_POT,
    GALARICA_CUFF,
    GALARICA_WREATH,
    LINKING_CORD,
    MALICIOUS_ARMOR,
    MASTERPIECE_TEACUP,
    METAL_ALLOY,
    PEAT_BLOCK,
    SCROLL_OF_DARKNESS,
    SCROLL_OF_WATERS,
    SWEET_APPLE,
    SYRUPY_APPLE,
    TART_APPLE,
    UNREMARKABLE_TEACUP,
    DEEP_SEA_SCALE,
    DEEP_SEA_TOOTH,
    DRAGON_SCALE,
    DUBIOUS_DISC,
    ELECTIRIZER,
    KINGS_ROCK,
    MAGMARIZER,
    METAL_COAT,
    OVAL_STONE,
    PRSIM_SCALE,
    PROTECTOR,
    RAZOR_CLAW,
    RAZOR_FANG,
    REAPER_CLOTH,
    SACHET,
    UPGRADE,
    WHIPPED_DREAM,
    STRAWBERRY_SWEET,
    LOVE_SWEET,
    BERRY_SWEET,
    CLOVER_SWEET,
    FLOWER_SWEET,
    STAR_SWEET,
    RIBBON_SWEET,
    FIRE_STONE,
    WATER_STONE,
    THUNDER_STONE,
    LEAF_STONE,
    MOON_STONE,
    SUN_STONE,
    SHINY_STONE,
    DUSK_STONE,
    DAWN_STONE,
    ICE_STONE
};

#endif