#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <set>
#include "symbol.hh"
#include "grammar.hh"

class Parser {

    int**               parseTable;
    Grammar*            grammar;

    std::set<Terminal*> first(Symbol* S);
    std::set<Terminal*> follow(Symbol* S);

public:
    Parser(Grammar* g);

    void                parse(std::vector<Terminal*>* inputTokenized);
    void                buildTable();
    void                serialize(const char* filename);
    void                deserialize(const char* filename);


};

#endif