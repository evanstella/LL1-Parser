#include "grammar.hh"
#include <vector>
#include <string>
#include <regex>



Terminal* Grammar::symbolLexer(std::string input) {

    std::vector<Terminal*>::iterator it;

    for (it = terminals.begin(); it != terminals.end(); it++) {
        if ((*(*it)).lexer(input)) 
            return (*it);
    }

    return NULL;
}

std::vector<Rule> Grammar::rulesFor(Variable V) {
    std::vector<Rule>::iterator it;

    for (it = rules.begin(); it != rules.begin(); it++) {
        //TODO
    }
 
}