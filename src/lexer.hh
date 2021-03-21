#ifndef LEXER_HH
#define LEXER_HH

#include "grammar.hh"
#include "symbol.hh"
#include <vector>

class Lexer {

    Grammar* G;
    bool     verbose;

public:

    Lexer(Grammar* G, bool verbose);
    std::vector<Terminal*>* tokenize(const char* inputRaw);

};

#endif