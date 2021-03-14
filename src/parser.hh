#ifndef PARSE_H
#define PARSE_H

#include <regex>
#include <string>
#include <stack>
#include <utility>

class Parser {

    std::vector<std::string>*           rules;
    std::stack<std::string>             stack; 
    std::pair<std::string, std::regex>* terminals;
    std::string*                        variables;
    std::string                         bos;
    int**                               parseTable;
    int                                 numTerminals, numVariables;

    int                                 lexer(std::string string);
    int                                 symbolToIndex(std::string symbol);

public:
    Parser();
    ~Parser();
    
    void parse(std::vector<std::string> inputTokenized);
};



#endif

