#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "symbol.hh"
#include "rule.hh"
#include <regex>

struct Grammar {

    std::vector<Terminal*>  terminals;
    std::vector<Variable*>  variables;
    std::vector<Rule>       rules;
    Variable*               startSymbol;
    Terminal*               bos = new Terminal(-2, 0, std::string("BOS"), std::regex("$"));
    Terminal*               epsilon = new Terminal(-1, -1, std::string("EPSILON"), std::regex("EPSILON"));

    std::vector<Rule>       rulesFor(Variable* V);
};

#endif