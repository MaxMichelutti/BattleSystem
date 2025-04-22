#ifndef __SPECIES_H__
#define __SPECIES_H__
#include "common.h"
#include "expcurve.h"
#include "enums.h"
#include "attack.h"
#include "abilities.h"
#include <map>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <optional>

class Species;
class Attack;
class AlternateForm;

// always contains a nullptr at index 0
static std::map<unsigned int,Species*> static_species;
static std::map<unsigned int,AlternateForm*> static_alternate_forms;

EvolutionMethod stringToEvolutionMethod(std::string);
std::string evolutionMethodToString(EvolutionMethod);

class Evolution{
    private:
    unsigned int target_species_id;
    unsigned int target_form_id;
    EvolutionMethod evo_method;
    unsigned int method_condition;
    public:
    Evolution();
    Evolution(unsigned int, unsigned int, EvolutionMethod, unsigned int);
    ~Evolution();
    unsigned int getTargetSpeciesId()const;
    EvolutionMethod getEvolutionMethod()const;
    unsigned int getMethodCondition()const;
    unsigned int getTargetFormId()const;
};

class AlternateForm{
    private:
    unsigned int species_id;
    FormKind form_kind;
    std::optional<Type> type1;
    std::optional<Type> type2;
    std::optional<Ability> ability1;
    std::optional<Ability> ability2;
    std::optional<Ability> hidden_ability;
    std::optional<Stats> basestats;
    std::optional<unsigned int> height_dm;
    std::optional<unsigned int> weight_hg;
    std::optional<unsigned int> base_friendship;
    std::optional<unsigned int> catch_rate;
    std::optional<unsigned int> exp_yield;
    std::optional<ExpCurve> exp_curve;
    std::optional<Stats> ev_yield;
    std::optional<std::vector<Evolution>> evolutions;
    std::optional<std::set<unsigned int>> evolution_learnset;
    std::optional<std::multimap<unsigned int,unsigned int>> learnset;
    void parseLearnset(std::string data);
    void parseEvolutions(std::string data);
    public:
    AlternateForm();
    AlternateForm(std::map<std::string,std::string> data);
    ~AlternateForm();
    unsigned int getSpeciesId()const;
    FormKind getFormKind()const;
    std::optional<Type> getType1()const;
    std::optional<Type> getType2()const;
    std::optional<Ability> getAbility1()const;
    std::optional<Ability> getAbility2()const;
    std::optional<Ability> getHiddenAbility()const;
    std::optional<Stats> getBaseStats()const;
    std::optional<unsigned int> getBaseFriendship()const;
    std::optional<unsigned int> getHeight()const;
    std::optional<unsigned int> getWeight()const;
    std::optional<unsigned int> getCatchRate()const;
    std::optional<unsigned int> getExpYield()const;
    std::optional<ExpCurve> getExpCurve()const;
    std::optional<Stats> getEVYield()const;
    std::optional<std::vector<Evolution>> getEvolutions()const;
    std::optional<std::set<unsigned int>> getEvolutionLearnset()const;
    std::optional<std::set<unsigned int>> getLearnsetUntil(unsigned int level)const;
    std::optional<std::set<unsigned int>> getLearnsetAt(unsigned int level)const;
    std::optional<std::multimap<unsigned int,unsigned int>> getLearnset()const;
    static AlternateForm* getAlternateForm(unsigned int species_id);
    static void loadAlternateForms();
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
    std::set<unsigned int> evolution_learnset;
    std::vector<unsigned int> alternate_forms_ids;

    void parseLearnset(std::string data);
    void parseEvolutions(std::string data);
    void checkAlternateForm(AlternateForm* form)const;
    std::set<unsigned int> getLearnsetAtInternal(unsigned int level);
    std::set<unsigned int> getLearnsetUntilInternal(unsigned int level);
    public:
    Species();
    Species(unsigned int index, std::map<std::string,std::string> data);
    ~Species();
    unsigned int getId()const;
    Stats getBaseStats(unsigned int form)const;
    Type getType1(unsigned int form)const;
    Type getType2(unsigned int form)const;
    std::string getName()const;
    static void loadSpecies();
    static Species* getSpecies(unsigned int species_id);
    ExpCurve getExpCurve(unsigned int form)const;
    GenderRule getGenderRule()const;
    unsigned int getBaseFriendship(unsigned int form)const;
    unsigned int getHeight(unsigned int form)const;
    unsigned int getWeight(unsigned int form)const;
    unsigned int getCatchRate(unsigned int form)const;
    unsigned int getExpYield(unsigned int form)const;
    Stats getEVYield(unsigned int form)const;
    Ability getAbility1(unsigned int form)const;
    Ability getAbility2(unsigned int form)const;
    Ability getHiddenAbility(unsigned int form)const;
    std::vector<Evolution> getEvolutions(unsigned int form)const;
    std::set<unsigned int> getEvolutionLearnset(unsigned int form)const;
    std::set<unsigned int> getLearnsetUntil(unsigned int form,unsigned int level);
    std::set<unsigned int> getLearnsetAt(unsigned int form,unsigned int level);
    void printSummary()const;
    static void printAllSummaries();
    void addAlternateFormId(unsigned int id);
    unsigned int getFormIdOfKind(FormKind form_kind)const;
    bool canMegaEvolve(unsigned int curr_form, ItemType mega_item)const;
    unsigned int getMegaForm(unsigned int curr_form, ItemType mega_item)const;
    unsigned int getNonMegaForm(unsigned int curr_form)const;
    bool hasForm(unsigned int form_id)const;
};


#endif