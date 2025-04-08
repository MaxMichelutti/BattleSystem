#ifndef __SPECIES_H__
#define __SPECIES_H__
#include "common.h"
#include "expcurve.h"
#include "enums.h"
#include "attack.h"
#include <map>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Species;
class Attack;

// always contains a nullptr at index 0
static std::map<unsigned int,Species*> static_species;

EvolutionMethod stringToEvolutionMethod(std::string);
std::string evolutionMethodToString(EvolutionMethod);

class Evolution{
    private:
    unsigned int target_species_id;
    EvolutionMethod evo_method;
    unsigned int method_condition;
    public:
    Evolution();
    Evolution(unsigned int, EvolutionMethod, unsigned int);
    ~Evolution();
    unsigned int getTargetSpeciesId()const;
    EvolutionMethod getEvolutionMethod()const;
    unsigned int getMethodCondition()const;
};

class Species{
    private:
    unsigned int id;
    Stats basestats;
    Type type1;
    Type type2;
    Ability ability1;
    Ability ability2;
    Ability hidden_ability;
    std::string name;
    std::multimap<unsigned int,unsigned int> learnset;// maps level to attack id
    ExpCurve exp_curve;
    GenderRule gender_rule;
    unsigned int base_friendship;
    unsigned int height_dm;
    unsigned int weight_hg;
    unsigned int catch_rate;
    unsigned int exp_yield;
    Stats ev_yield;
    std::vector<Evolution> evolutions;
    std::vector<unsigned int> evolution_learnset;

    void parseLearnset(std::string data);
    void parseEvolutions(std::string data);
    public:
    Species();
    Species(unsigned int index, std::map<std::string,std::string> data);
    ~Species();
    unsigned int getId()const;
    Stats getBaseStats()const;
    Type getType1()const;
    Type getType2()const;
    std::string getName()const;
    static void loadSpecies();
    static Species* getSpecies(unsigned int species_id);
    ExpCurve getExpCurve()const;
    GenderRule getGenderRule()const;
    unsigned int getBaseFriendship()const;
    unsigned int getHeight()const;
    unsigned int getWeight()const;
    unsigned int getCatchRate()const;
    unsigned int getExpYield()const;
    Stats getEVYield()const;
    Ability getAbility1()const;
    Ability getAbility2()const;
    Ability getHiddenAbility()const;
    std::vector<Evolution> getEvolutions()const;
    std::set<unsigned int> getEvolutionLearnset()const;
    std::set<unsigned int> getLearnsetUntil(unsigned int level);
    std::set<unsigned int> getLearnsetAt(unsigned int level);
    void printSummary()const;
    static void printAllSummaries();
};


#endif