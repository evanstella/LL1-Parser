#include "rule.hh"
#include <vector>

Rule::Rule(Variable lhs, std::vector<Symbol*> rhs): LHS(lhs), RHS(rhs) {}