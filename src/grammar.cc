#include "grammar.hh"
#include <vector>
#include <string>
#include <regex>



Terminal* Grammar::symbolLexer(std::string input) {

    std::vector<Terminal*>::iterator it = terminals.begin();

    for (it = terminals.begin(); it != terminals.end(); it++) {
        if ((*(*it)).lexer(input)) 
            return (*it);
    }

    return NULL;
}