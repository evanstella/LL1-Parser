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

    Variable E = Variable(0, 0, std::string("E"));
    Variable Ep = Variable(1, 1, std::string("E'"));
    Variable T = Variable(2, 2, std::string("T"));
    Variable Tp = Variable(3, 3, std::string("T'"));
    Variable F = Variable(4, 4, std::string("F"));
    Terminal p1 = Terminal(5, 4, std::string("("), std::regex("\\("));
    Terminal p2 = Terminal(6, 5, std::string(")"), std::regex("\\)"));
    Terminal pl = Terminal(7, 2, std::string("+"), std::regex("\\+"));
    Terminal id = Terminal(8, 1, std::string("id"), std::regex("id"));
    Terminal as = Terminal(9, 3, std::string("*"), std::regex("as"));

    G.terminals = std::vector<Terminal*>{G.bos,&p1,&p2,&id,&as,&pl};
    G.variables = std::vector<Variable*>{&E,&Ep,&T,&Tp,&F};
    G.startSymbol = &E;
    
    Rule r1 = Rule(&E, std::vector<Symbol*>{&T,&Ep});
    Rule r2 = Rule(&Ep, std::vector<Symbol*>{&pl,&T, &Ep});
    Rule r3 = Rule(&Ep, std::vector<Symbol*>{G.epsilon});
    Rule r4 = Rule(&T, std::vector<Symbol*>{&F,&Tp});
    Rule r5 = Rule(&Tp, std::vector<Symbol*>{&as,&F,&Tp});
    Rule r6 = Rule(&Tp, std::vector<Symbol*>{G.epsilon});
    Rule r7 = Rule(&F, std::vector<Symbol*>{&p1,&E,&p2});
    Rule r8 = Rule(&F, std::vector<Symbol*>{&id});

    G.rules = std::vector<Rule>{r1,r2,r3,r4,r5,r6,r7,r8};

    Parser p = Parser(&G);

    //std::vector<Terminal*> input = {&p1,&a,&pl,&a,&p2,G.bos};
        
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

}


Parser::~Parser() {
    for (unsigned int i = 0; i < grammar->variables.size(); i++)
        delete[] parseTable[i];
    delete[] parseTable;
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

    // print for debug
    for (unsigned int i = 0; i < grammar->variables.size(); i++) {
        for (unsigned int j = 0; j < grammar->terminals.size(); j++) {
            if (parseTable[i][j] != -1)
                std::cout << parseTable[i][j] << " ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
}