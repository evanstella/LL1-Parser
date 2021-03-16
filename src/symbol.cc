#include "symbol.hh"
#include <regex>
#include <string>


Symbol::Symbol(int tag, int index, std::string id): tag(tag), index(index), identifier(id) {}

Symbol::~Symbol() {}

int Symbol::getTag() {return tag;}

std::string Symbol::getId() {return identifier;}

int Symbol::getIndex() {return index;}



Variable::Variable(int tag, int index, std::string id): Symbol(tag, index, id) {}

SymbolType Variable::type() {return SymbolType(VAR);}

bool Variable::lexer(std::string token) {return -1;}



Terminal::Terminal(int tag, int index, std::string id, std::regex pattern): Symbol(tag, index, id), pattern(pattern) {}

SymbolType Terminal::type() {return SymbolType(TERM);}

bool Terminal::lexer(std::string token) {return (std::regex_match(token, pattern));}