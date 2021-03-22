#include "lexer.hh"
#include <string>
#include <regex>
#include <iostream>

Lexer::Lexer(Grammar* G, bool verbose): G(G), verbose(verbose) {}


std::vector<Terminal*>* Lexer::tokenize(const char* inputRaw) {

    auto tokens = new std::vector<Terminal*>;
    int i = 0;
    char current = inputRaw[i++];
    Terminal whitespace = Terminal(-10,-10,"",std::regex("\\s+"));
    

    std::vector<Terminal*> possibleMatches = G->terminals;
    possibleMatches.push_back(&whitespace);
    
    std::string token = "";

    if (verbose)
        std::cout << "TOKENS:" << std::endl;

    // search input for longest match
    while (current) {
        // check for possible matches
        std::string tmp = token;
        tmp += current;
        int matches = 0;

        for (auto it = possibleMatches.begin(); it != possibleMatches.end(); it++) {
            if ((*it)->lexer(tmp))
                matches++;
        }

        // if matches, continue adding to token
        // otherwise, validate token and add to tokens
        // if invalid token: error
        if (matches != 0) {
            token += current;
            current = inputRaw[i++];
        }
        else {
            Terminal* t = G->matchTerminal(token);
            if (t) {
                if (verbose)
                    std::cout << token << ":\t" << t->getId() << std::endl;
                tokens->push_back(t);
            }
            else if (!whitespace.lexer(token)) {
                std::cout << "Invalid Token: " << token << std::endl;
                std::cout << "INPUT REJECTED" << std::endl;
                exit(0);
            }
            token = std::string(1, current);
            current = inputRaw[i++];
        }

        // special case for last token
        if (current == 0) {
            Terminal* t = G->matchTerminal(token);
            if (t) {
                if (verbose)
                    std::cout << token << ":\t" << t->getId() << std::endl;
                tokens->push_back(t);
            }
            else if (!whitespace.lexer(token)) {
                std::cout << "Invalid Token: " << token << std::endl;
                exit(0);
            }
        }
    }

    if (verbose)
        std::cout << std::endl;

    tokens->push_back(G->bos);
    return tokens;

}