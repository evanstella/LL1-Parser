#ifndef RULE_H
#define RULE_H

#include <vector>
#include "symbol.hh"

struct Rule {

    Variable                LHS;
    std::vector<Symbol*>    RHS;

    Rule(Variable lhs, std::vector<Symbol*> rhs);

};

#endif 