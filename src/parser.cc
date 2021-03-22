#include "parser.hh"
#include "grammar.hh"
#include "symbol.hh"
#include "rule.hh"
#include "lexer.hh"
#include <stack>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <fstream>


Parser::Parser(Grammar* g, bool verbose): grammar(g), verbose(verbose) {}


void Parser::parse(std::vector<Terminal*>* inputTokenized) {
    std::stack<Symbol*> pda = std::stack<Symbol*>();
    Grammar* G = grammar;

    // keep track of current position for error message
    int currentPos = 0;
    std::vector<Terminal*>::iterator it = (*inputTokenized).begin();

    pda.push(G->bos);
    pda.push(G->startSymbol);

    if (verbose)
        std::cout << "PDA:" << std::endl;

    while(pda.size() > 0) {

        // compare input with TOS
        Terminal* token = *it;
        Symbol* top = pda.top();
        if (token->getTag() == (top->getTag())) {
            // TOS matches input
            if (verbose)
                std::cout << "Matched symbols: " << top->getId() << std::endl;
            currentPos++;
            it++;
            pda.pop();
        }
        else {
            // TOS doesnt match, push symbols for rule(TOS) onto stack
            if (top->type() == SymbolType(TERM)) {
                std::cout << "Error at position " << currentPos << std::endl;
                std::cout << "\nINPUT REJECTED" << std::endl;
                exit(0);
            }
            // get rule from table for [variable][terminal]
            int ruleNum = parseTable[top->getIndex()][token->getIndex()];
            if (verbose)
                std::cout << "rule " << ruleNum << std::endl;
            if (ruleNum == -1) {
                std::cout << "Error at position " << currentPos << std::endl;
                std::cout << "\nINPUT REJECTED" << std::endl;
                exit(0);
            }
            Rule rule = G->rules[ruleNum];
            pda.pop();
            if (!rule.epsilonIn(grammar->epsilon)) {
                // push each symbol for rule
                for (std::vector<Symbol*>::reverse_iterator it = rule.RHS.rbegin(); it != rule.RHS.rend(); it++) {
                    pda.push(*it);
                }
            }
        }
    }
    if (verbose)
        std::cout << std::endl;

    std::cout << "INPUT ACCEPTED" << std::endl;
}


std::set<Terminal*> Parser::first(Symbol* S) {

    // If A is a terminal, FIRST(A) = {A}.
    if (S->type() == SymbolType(TERM)) {
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

    // for A -> Y1Y2Y3..Yk, from 1 to k: if Y1Y2...Yi−1 ∗=> eps) 
    // and a in FIRST(Yi), then add a to FIRST(A).
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
            // Y1Y2...Yj−1 *=> eps) and a in FIRST(Yj), then add a to FIRST(A)
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


std::set<Terminal*> Parser::follow(Symbol* S) {
    std::set<Terminal*> ret;
    // $ is in FOLLOW(Start)
    if (S == grammar->startSymbol) {
        ret.insert(grammar->bos);
    }

    std::vector<Rule> rulesS = grammar->rulesHave(S);

    // for each rule A -> ...S...
    for (unsigned int i = 0; i < rulesS.size(); i++) {
        std::vector<Symbol*>::iterator posS = std::find(rulesS[i].RHS.begin(), rulesS[i].RHS.end(), S);
        bool addFollowA = false;  // could have to add follow(A) twice, inefficient to do so

        // if A -> αSB, then FIRST(B) − {eps} proper subset of FOLLOW(S).
        if (posS != --rulesS[i].RHS.end()) {
            auto set = first(*(++posS));
            bool containsEps = false;
            // add each of FIRST(C) - {eps} to FOLLOW(S)
            for (std::set<Terminal*>::iterator it = set.begin(); it != set.end(); it++) {
                if (*it != grammar->epsilon)
                    ret.insert(*it);
                else
                    containsEps = true;
            }
            // if A -> aSB where eps in FIRST(B) (i.e., B *=> eps), FOLLOW(A) proper subset of FOLLOW(S)
            if (containsEps) {
                addFollowA = true;
            }
        }
        else {
            // if A -> aS FOLLOW(A) proper subset of FOLLOW(S)
            addFollowA = true;
        }

        if (addFollowA) {
            auto set = follow(rulesS[i].LHS);
            for (std::set<Terminal*>::iterator it = set.begin(); it != set.end(); it++) {
                ret.insert(*it);
            }
        }
    }
    
    return ret;
}


void Parser::buildTable() {
    //clear table
    parseTable = new int*[grammar->variables.size()];
    for (unsigned int i = 0; i < grammar->variables.size(); i++)
        parseTable[i] = new int[grammar->terminals.size()];

    for (unsigned int i = 0; i < grammar->variables.size(); i++)
        for (unsigned int j = 0; j < grammar->terminals.size(); j++)
            parseTable[i][j] = -1;

    // for each rule...
    for (unsigned int i = 0; i < grammar->rules.size(); i++) {
        bool containsEps = false;
        bool containsBOS = false;

        // for each terminal t in FIRST(a), add A -> a... to M[A, t]
        Symbol* a = grammar->rules[i].RHS[0];
        Variable* A = grammar->rules[i].LHS;
        auto set = first(a);
        for (std::set<Terminal*>::iterator t = set.begin(); t != set.end(); t++) {
            if (*t == grammar->epsilon)
                containsEps = true;
            else 
                parseTable[A->getIndex()][(*t)->getIndex()] = i;
        }

        // if eps in FIRST(a), add A -> a... to M[A, b] for each terminal b ∈ FOLLOW(A)
        if (containsEps) {
            set = follow(A);
            for (std::set<Terminal*>::iterator b = set.begin(); b != set.end(); b++) {
                if (*b == grammar->bos)
                    containsBOS = true;
                else 
                    parseTable[A->getIndex()][(*b)->getIndex()] = i;
            }

            // if eps in FIRST(a) and $ in FOLLOW(A), add A -> a... to M[A, $]
            if (containsBOS) {
                parseTable[A->getIndex()][grammar->bos->getIndex()] = i;
            }
        }
    }
}


void Parser::serialize(const char* filename) {

    std::ofstream file;
    file.open(filename, std::fstream::binary | std::fstream::out);

    if (!file.is_open()) {
        std::cerr << "Error opening parser file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // serialize parseTable
    int tmp = grammar->variables.size();
    file.write((char*) &tmp, sizeof(int));
    tmp = grammar->terminals.size()-1;
    file.write((char*) &tmp, sizeof(int));
    tmp = grammar->rules.size();
    file.write((char*) &tmp, sizeof(int));
    
    for (int i = 0; i < grammar->variables.size(); i++)
        for (int j = 0; j < grammar->terminals.size(); j++) {
            file.write((char*) &parseTable[i][j], sizeof(int));
        }

    // serialize terminals
    for (int i = 0; i < grammar->terminals.size(); i++) {
        Terminal* t = grammar->terminals[i];
        if (t->getTag() != grammar->bos->getTag()) {
            int tag = t->getTag();
            int index = t->getIndex();
            int size = t->getId().size();
            char s[size+1];
            strcpy(s, t->getId().c_str());
            file.write((char*) &tag, sizeof(int));
            file.write((char*) &index, sizeof(int));
            file.write((char*) &size, sizeof(int));
            file.write(s, size+1);
        }
    }

    // serialize variables
    for (int i = 0; i < grammar->variables.size(); i++) {
        Variable* v = grammar->variables[i];
        int tag = v->getTag();
        int index = v->getIndex();
        int size = v->getId().size();
        char s[size+1];
        strcpy(s, v->getId().c_str());
        file.write((char*) &tag, sizeof(int));
        file.write((char*) &index, sizeof(int));
        file.write((char*) &size, sizeof(int));
        file.write(s, size+1);
    }

    // serialize rules
    for (int i = 0; i < grammar->rules.size(); i++) {
        Rule r = grammar->rules[i];
        int lhs = r.LHS->getTag();
        int size = r.RHS.size();
        file.write((char*) &lhs, sizeof(int));
        file.write((char*) &size, sizeof(int));
        for (int j = 0; j < r.RHS.size(); j++) {
            int tag = r.RHS[j]->getTag();
            file.write((char*) &tag, sizeof(int));
        }
    }

    // serialize remaining grammar parts
    int tag = grammar->startSymbol->getTag();
    file.write((char*) &tag, sizeof(int));

    file.close();
}


void Parser::deserialize(const char* filename) {

    std::ifstream file;
    file.open(filename, std::fstream::binary | std::fstream::in);

    if (!file.is_open()) {
        std::cerr << "Error opening parser file" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (grammar == NULL) {
        std::cerr << "No grammar to initialize" << std::endl;
        exit(EXIT_FAILURE);
    }

    int numTerms, numVars, numRules;

    file.read((char*) &numTerms, sizeof(int));
    file.read((char*) &numVars,  sizeof(int));
    file.read((char*) &numRules, sizeof(int));

    parseTable = new int*[numVars];
    for (unsigned int i = 0; i < numVars; i++)
        parseTable[i] = new int[numTerms+1];

    // deserialize parse table
    for (int i = 0; i < numVars; i++)
        for (int j = 0; j < numTerms+1; j++) {
            file.read((char*) &parseTable[i][j], sizeof(int));
        }

    // deserialize terminals
    for (int i = 0; i < numTerms; i++) {
        int tag, index, size;
        std::string id;
        file.read((char*) &tag, sizeof(int));
        file.read((char*) &index, sizeof(int));
        file.read((char*) &size, sizeof(int));
        char tmp[size+1];
        file.read(tmp, size+1);
        id.assign(tmp, size);
        grammar->terminals.push_back(new Terminal(tag, index, id, std::regex(id.substr(1,id.length()-2))));
    }

    // deserialize variables
    for (int i = 0; i < numVars; i++) {
        int tag, index, size;
        std::string id;
        file.read((char*) &tag, sizeof(int));
        file.read((char*) &index, sizeof(int));
        file.read((char*) &size, sizeof(int));
        char tmp[size+1];
        file.read(tmp, size+1);
        id.assign(tmp, size+1);
        grammar->variables.push_back(new Variable(tag, index, id));
    }

    // deserialize rules
    for (int i = 0; i < numRules; i++) {
        Rule r;
        int tag, size;
        file.read((char*) &tag, sizeof(int));
        r.LHS = (Variable*) grammar->getSymbol(tag);
        if (r.LHS == NULL) {
            std::cerr << "Error reading parser file" << std::endl;
            exit(EXIT_FAILURE);
        }
        file.read((char*) &size, sizeof(int));
        for (int j = 0; j < size; j++) {
            file.read((char*) &tag, sizeof(int));
            Symbol* s = grammar->getSymbol(tag);
            if (s == NULL) {
                std::cerr << "Error reading parser file" << std::endl;
                exit(EXIT_FAILURE);
            }
            r.RHS.push_back(s);
        }
        grammar->rules.push_back(r);
    }

    int tag;
    file.read((char*) &tag, sizeof(int));
    Variable* s = (Variable*) grammar->getSymbol(tag);
    if (s == NULL) {
        std::cerr << "Error reading parser file" << std::endl;
        exit(EXIT_FAILURE);
    }
    grammar->startSymbol = s;
    grammar->terminals.push_back(grammar->bos);

    file.close();

}