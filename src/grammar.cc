#include "grammar.hh"
#include "symbol.hh"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>


// get all rules such that V -> A...
 std::vector<Rule> Grammar::rulesFor(Variable* V) {
    std::vector<Rule> r;
    for (unsigned int i = 0; i < rules.size(); i++) {
        if (rules[i].LHS == V) {
            r.push_back(rules[i]);
        }
    }
    return r;
 }

// get all rules such that A -> ...S...
std::vector<Rule> Grammar::rulesHave(Symbol* S) {
    std::vector<Rule> r;
    for (unsigned int i = 0; i < rules.size(); i++) {
        for (unsigned int j = 0; j < rules[i].RHS.size(); j++) {
            if (rules[i].RHS[j] == S) {
                if (S->type() == SymbolType(VAR) && rules[i].LHS != (Variable*)S) {
                    r.push_back(rules[i]);
                    break;
                }
            }
        }
    }
    return r;
}


// read grammar from a file
void Grammar::readGrammar(const char* filename) {

    int counter = 0;
    Variable* currentVar;
    Rule currentRule;

    std::fstream file;
    file.open(filename, std::fstream::in);

    if (!file.is_open()) {
        std::cerr << "Error opening grammar file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(file, line)) {
        if (line.length() == 0)
            continue;
        
        std::istringstream ss(line);
        std::string token;

        ss >> token;
        if (token == "|") {
            if (!currentRule.RHS.empty())
                rules.push_back(currentRule);
            currentRule = Rule();
            currentRule.LHS = currentVar;

            while (ss >> token) {
                if (token.at(0) == '<') {
                    bool found = false;
                    for (unsigned int i = 0; i < terminals.size(); i++) {
                        if (terminals[i]->getId() == token) {
                            currentRule.RHS.push_back(terminals[i]);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        auto term = new Terminal (counter++, terminals.size()+1, token, std::regex(token.substr(1,token.length()-2)));
                        terminals.push_back(term);
                        currentRule.RHS.push_back(term);
                    }
                }
                else {
                    bool found = false;
                    if (token == "EPSILON") {
                        currentRule.RHS.push_back(epsilon);
                    } 
                    else {
                        for (unsigned int i = 0; i < variables.size(); i++) {
                            if (variables[i]->getId() == token) {
                                currentRule.RHS.push_back(variables[i]);
                                found = true;
                                break;
                            }
                        }
                        if (!found) {  
                            auto var = new Variable (counter++, variables.size(), token);
                            variables.push_back(var);
                            currentRule.RHS.push_back(var);
                        }
                    }
                }
            }
        }
        else {
            if (!currentRule.RHS.empty())
                rules.push_back(currentRule);
            currentRule = Rule();
            bool found = false;
            for (unsigned int i = 0; i < variables.size(); i++) {
                if (variables[i]->getId() == token) {
                    currentRule.LHS = variables[i];
                    found = true;
                    currentVar = variables[i];
                    break;
                }
            }
            if (!found) {  
                auto var = new Variable (counter++, variables.size(), token);
                variables.push_back(var);
                currentRule.LHS = var;
                currentVar = var;
            }
            
        }
    }
    rules.push_back(currentRule);
    startSymbol = variables[0];
    terminals.push_back(bos);
    file.close();
}


void Grammar::printRules() {

    std::cout << "RULES:" << std::endl;
    for (unsigned int i = 0; i < rules.size(); i++) {
        std::cout << i << ". " << rules[i].LHS->getId() << "\t-> ";
        for (unsigned int j = 0; j < rules[i].RHS.size(); j++) {
            std::cout << rules[i].RHS[j]->getId() << " ";
        } 
        std::cout << std::endl;
    }
    std::cout << std::endl;

}


Symbol* Grammar::getSymbol(int tag) {
    if (tag == -1)
        return epsilon;
    for (unsigned int i = 0; i < terminals.size(); i++) {
        if (terminals[i]->getTag() == tag)
            return (Symbol*) terminals[i];
    }
    for (unsigned int i = 0; i < variables.size(); i++) {
        if (variables[i]->getTag() == tag)
            return (Symbol*) variables[i];
    }
    return NULL;
}


Terminal* Grammar::matchTerminal(std::string s) {
    for (Terminal* t: terminals) {
        if (t->lexer(s)) 
            return t;
    }

    return NULL;
}