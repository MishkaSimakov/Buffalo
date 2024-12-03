#include "Grammar.h"

void Grammar::extend() {
  NonTerminal new_start;
  add_rule(new_start, start_);
  start_ = new_start;
}

void Grammar::remove_epsilon_rules() {}

void Grammar::remove_unreachable() {}

void Grammar::add_rule(NonTerminal from, GrammarProductionResult to) {
  productions_[from].push_back(std::move(to));
}
