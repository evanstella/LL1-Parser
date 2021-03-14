#include <iostream>
#include <string>
#include <regex>
#include <utility>
#include "parser.hh"

int main(void) {
    Parser p = Parser();
    std::vector<std::string> input;
    input.push_back(std::string("("));
    input.push_back(std::string("a"));
    input.push_back(std::string("+"));
    input.push_back(std::string("a"));
    input.push_back(std::string(")"));
    input.push_back(std::string(")"));
    input.push_back(std::string("BOS"));


    p.parse(input);
}

Parser::Parser() {

    parseTable = new int*[2];
    for (int i = 0; i < 2; i++)
        parseTable[i] = new int[5];

    bos = std::string("BOS");
    numTerminals = 5;
    numVariables = 2;

    for (int i = 0; i < numVariables; i++)
        for (int j = 0; j < numTerminals; j++) {
            parseTable[i][j] = 0;
        }

    parseTable[0][0] = 2;
    parseTable[0][2] = 1;
    parseTable[1][2] = 3;

    rules = new std::vector<std::string>[3];
    rules[0].push_back(std::string("F"));
    rules[1].push_back(std::string("("));
    rules[1].push_back(std::string("S"));
    rules[1].push_back(std::string("+"));
    rules[1].push_back(std::string("F"));
    rules[1].push_back(std::string(")"));
    rules[2].push_back(std::string("a"));

    terminals = new std::pair<std::string, std::regex>[numTerminals];
    terminals[0].first = std::string("(");
    terminals[0].second = std::regex("\\(");
    terminals[1].first = std::string(")");
    terminals[1].second = std::regex("\\)");
    terminals[2].first = std::string("a");
    terminals[2].second = std::regex("a");
    terminals[3].first = std::string("+");
    terminals[3].second = std::regex("\\+");
    terminals[4].first = bos;
    terminals[4].second = std::regex(bos);

    variables = new std::string[numVariables];
    variables[0] = std::string("S");
    variables[1] = std::string("F");
}


Parser::~Parser() {
    delete[] rules;
    delete[] terminals;
    delete[] variables;

    for (int i = 0; i < numVariables; i++) {
        delete parseTable[i];
    }

    delete[] parseTable;
}


void Parser::parse(std::vector<std::string> inputTokenized) {
    // initialize stack
    stack.push(bos); 
    stack.push(variables[0]);

    int currentPos = 0;  // track where an error occured
    std::vector<std::string>::iterator token = inputTokenized.begin();

    while(stack.size() > 0) {
        
        if (token == inputTokenized.end())
            std::cout << "Error At Position " << currentPos << ": " << *(--token) << std::endl;
        int term = lexer(*token);
        if (term == -1) {
            std::cout << "Error At Position " << currentPos << ": " << *token << std::endl;
            exit(0);
        } 
        else if (terminals[term].first == stack.top()) {
            // std::cout << "Matched symbols: " << terminals[term].first << std::endl;
			token++;
			stack.pop();
        }
        else {
            int symbol = symbolToIndex(stack.top());
            int ruleNum = parseTable[symbol][term];
            std::cout << "Rule " << ruleNum << std::endl;
            std::vector<std::string> rule = rules[ruleNum-1];
            if (ruleNum == 0) {
                std::cout << "Error At Position " << currentPos << ": " << *token << std::endl;
                exit(0);
            }
            stack.pop();
            for (std::vector<std::string>::reverse_iterator it = rule.rbegin(); it != rule.rend(); it++) {
                stack.push(*it);
            }
        }
    }

    std::cout << "Input Accepted" << std::endl;
}


/* 
    returns the index of the terminals array which the
    string matches, -1 if no match 
*/
int Parser::lexer(std::string string) {

    for (int i = 0; i < numTerminals; i++)
        if (std::regex_match(string, terminals[i].second))
            return i;

    return -1;
}   



int Parser::symbolToIndex(std::string symbol) {
    for (int i = 0; i < numVariables; i++) {
        if (variables[i] == symbol)
            return i;
    }
    
    return -1;
}




