#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include "symbol.hh"
#include "grammar.hh"

class Parser {

    int**               parseTable;
    Grammar*            grammar;

public:
    Parser(Grammar* g);

    void                parse(std::vector<std::string>* inputTokenized);

};

#endif