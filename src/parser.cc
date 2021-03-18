#include "parser.hh"
#include "grammar.hh"
#include "symbol.hh"
#include "rule.hh"
#include <stack>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>


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
    
    Rule r1 = Rule(&S, std::vector<Symbol*>{&F});
    Rule r2 = Rule(&S, std::vector<Symbol*>{&p1, &S, &pl, &F, &p2});
    Rule r3 = Rule(&F, std::vector<Symbol*>{&a});

    G.rules = std::vector<Rule>{r1,r2,r3};

    Parser p = Parser(&G);

    std::vector<Terminal*> input = {&p1,&a,&pl,&a,&p2,G.bos};
        
    p.buildTable();

    //p.parse(&input);
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


std::set<Terminal*> Parser::first(Symbol* S) {

    // If A is a terminal, FIRST(A) = {A}.
    if (S->type() == SymbolType(TERM)) {
        std::cout << S->getId() << std::endl;
        return std::set<Terminal*>{(Terminal*) S};
    }
    std::set<Terminal*> ret;
    Variable* V = (Variable*) S;
    std::vector<Rule> rulesV = grammar->rulesFor(V);
    // If A -> eps, add eps to FIRST(A)
    for (unsigned int i = 0; i < rulesV.size(); i++) {
        if (rulesV[i].epsilonIn(grammar->epsilon)) {
            ret.insert(grammar->epsilon);
        }
    }

    for (unsigned int i = 0; i < rulesV.size(); i++){
        for (unsigned int j = 0; j < rulesV[i].RHS.size(); j++) {
            std::set<Terminal*> set;
            bool containsEps = true;
            for (unsigned int k = 0; k < j; k++) {
                set = first(rulesV[i].RHS[k]);
                if (set.find(grammar->epsilon) == set.end()) {
                    containsEps = false;
                    break;
                }
            }
            // Y1Y2...Yj−1 ∗⇒ eps) and a ∈ FIRST(Yj), then add a to FIRST(A)
            if (containsEps) {
                //add elements of set to ret
                set = first(rulesV[i].RHS[j]);
                for (std::set<Terminal*>::iterator it = set.begin(); it != set.end(); it++) {
                    ret.insert(*it);
                }
            }
        }
    }

    return ret;
}


void Parser::buildTable() {
    //clear table
    for (unsigned int i = 0; i < grammar->variables.size(); i++)
        for (unsigned int j = 0; j < grammar->terminals.size(); j++)
            parseTable[i][j] = -1;

    for (unsigned int i = 0; i < grammar->rules.size(); i++) {
        Symbol* alpha = grammar->rules[i].RHS[0];
        Variable* A = grammar->rules[i].LHS;
        auto set = first(alpha);
        std::cout << "test" << std::endl;
        for (std::set<Terminal*>::iterator it = set.begin(); it != set.end(); it++) {
            parseTable[A->getIndex()][(*it)->getIndex()] = i;
        }
    }

    for (unsigned int i = 0; i < grammar->variables.size(); i++) {
        for (unsigned int j = 0; j < grammar->terminals.size(); j++)
            std::cout << parseTable[i][j] << " ";
        std::cout << std::endl;
    }
}