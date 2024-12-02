#include "Grammar.h"

void Grammar::extend() {
  auto& extended_rules = rules_[0];
  extended_rules.resize(1);
  extended_rules[0] = {cExtendedGrammarStart, std::string(cGrammarStart, 1)};
}

void Grammar::remove_epsilon_rules() {

}

void Grammar::remove_unreachable() {

}

void Grammar::add_rule(char from, std::string_view to) {
  if (!is_acceptable_non_terminal(from)) {
    throw std::runtime_error("Only A-Z may appear as non-terminals");
  }
  for (char symbol : to) {
    if (!is_acceptable_non_terminal(symbol) &&
        !is_acceptable_terminal(symbol)) {
      throw std::runtime_error(
          "Only a-z or A-Z may appear in right part of rules.");
    }
  }

  Rule new_rule;
  new_rule.left = from;
  new_rule.right = to;
  new_rule.right += cRuleEndSymbol;

  rules_[from - cExtendedGrammarStart].push_back(std::move(new_rule));
}
