#include "grammar.hh"
#include "symbol.hh"
#include <vector>
#include <iostream>
#include <string>
#include <regex>




 std::vector<Rule> Grammar::rulesFor(Variable* V) {
    std::vector<Rule> r;
    for (unsigned int i = 0; i < rules.size(); i++) {
        if (rules[i].LHS == V) {
            r.push_back(rules[i]);
        }
    }
    return r;
 }