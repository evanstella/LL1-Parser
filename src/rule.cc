#include "rule.hh"
#include <vector>

Rule::Rule(Variable* lhs, std::vector<Symbol*> rhs): LHS(lhs), RHS(rhs) {}

 bool Rule::epsilonIn(Symbol* epsilon) {
    return (RHS.size() == 1 && RHS[0] == epsilon);
}