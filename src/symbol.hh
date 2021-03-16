#ifndef SYMBOL_H
#define SYMBOL_H

#include <regex>
#include <string>


enum SymbolType {VAR, TERM};

class Symbol {

    int                 tag;        // for quick compares
    int                 index;      // index in parse table; 
    std::string         identifier; // for output to user

public:
    Symbol(int tag, int index, std::string id);

    std::string         getId();
    int                 getTag();
    int                 getIndex();

    virtual SymbolType  type() = 0;
    virtual bool        lexer(std::string token) = 0;

};

class Variable: public Symbol {

public:
    Variable(int tag, int index, std::string id);

    SymbolType          type();
    bool                lexer(std::string token);

};

class Terminal: public Symbol {

    std::regex      pattern;

public:
    Terminal(int tag, int index, std::string id, std::regex pattern);

    SymbolType          type();
    bool                lexer(std::string token);

};


#endif