#include "parser.hh"
#include "grammar.hh"
#include "symbol.hh"
#include "rule.hh"
#include <stack>
#include <iostream>
#include <vector>
#include <string>
#include <regex>


int main (void) {

    Grammar G = Grammar();

    Variable S = Variable(0, 0, std::string("S"));
    Variable F = Variable(1, 1, std::string("F"));
    Terminal p1 = Terminal(2, 1, std::string("("), std::regex("\\("));
    Terminal p2 = Terminal(3, 2, std::string(")"), std::regex("\\)"));
    Terminal a = Terminal(4, 3, std::string("a"), std::regex("a"));
    Terminal pl = Terminal(5, 4, std::string("+"), std::regex("\\+"));

    G.terminals = std::vector<Terminal*>{&p1,&p2,&a,&pl,G.bos};
    G.variables = std::vector<Variable*>{&S,&F};
    G.startSymbol = &S;
    
    Rule r1 = Rule(S, std::vector<Symbol*>{&F});
    Rule r2 = Rule(S, std::vector<Symbol*>{&p1, &S, &pl, &F, &p2});
    Rule r3 = Rule(S, std::vector<Symbol*>{&a});

    G.rules = std::vector<Rule>{r1,r2,r3};

    Parser p = Parser(&G);

    std::vector<Terminal*> input = {&p1,&a,&pl,&a,&p2,G.bos};
        

    p.parse(&input);
}


Parser::Parser(Grammar* g): grammar(g) {

    parseTable = new int*[g->variables.size()];
    for (unsigned int i = 0; i < g->variables.size(); i++)
        parseTable[i] = new int[g->terminals.size()];

    for (unsigned int i = 0; i < g->variables.size(); i++)
        for (unsigned int j = 0; j < g->terminals.size(); j++) {
            parseTable[i][j] = -1;
        }

    parseTable[0][1] = 1;
    parseTable[0][3] = 0;
    parseTable[1][3] = 2;
}


void Parser::parse(std::vector<Terminal*>* inputTokenized) {
    std::stack<Symbol*> pda = std::stack<Symbol*>();
    Grammar* G = grammar;

    // keep track of current position for error message
    int currentPos = 0;
    std::vector<Terminal*>::iterator it = (*inputTokenized).begin();

    pda.push(G->bos);
    pda.push(G->startSymbol);

    while(pda.size() > 0) {

        // compare input with TOS
        Terminal* token = *it;
        Symbol* top = pda.top();
        if (token->getTag() == (top->getTag())) {
            // TOS matches input
            std::cout << "Matched symbols: " << top->getId() << std::endl;
            currentPos++;
            it++;
            pda.pop();
        }
        else {
            // TOS doesnt match, push symbols for rule(TOS) onto stack
            if (top->type() == SymbolType(TERM)) {
                std::cout << "(2) Error at position " << currentPos << ": \"" << token->getId() << "\" :: Expected: " << top->getId() << std::endl;
                exit(0);
            }
            // get rule from table for [variable][terminal]
            int ruleNum = parseTable[top->getIndex()][token->getIndex()];
            std::cout << "Rule " << ruleNum << std::endl;
            Rule rule = G->rules[ruleNum];
            if (ruleNum == -1) {
                std::cout << "(3) Error at position " << currentPos << ": \"" << token->getId() << "\" :: Expected: " << top->getId() << std::endl;
                exit(0);
            }
            pda.pop();
            // push each symbol for rule
            for (std::vector<Symbol*>::reverse_iterator it = rule.RHS.rbegin(); it != rule.RHS.rend(); it++) {
                pda.push(*it);
            }
        }
    }
    std::cout << "Input Accepted" << std::endl;
}