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
    std::set<Terminal*> follow();

public:
    Parser(Grammar* g);
    ~Parser();

    void                parse(std::vector<Terminal*>* inputTokenized);
    void                buildTable();


};

#endif