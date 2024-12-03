#pragma once

#include <string_view>
#include <vector>

#include "Grammar.h"

class GrammarBuilder {
  static constexpr char cStartNonTerminal = 'S';

  std::vector<std::pair<char, std::string_view>> productions_;

  static bool is_valid_terminal(char symbol);
  static bool is_valid_non_terminal(char symbol);
  static GrammarProductionResult string_to_production(
      std::string_view string,
      std::unordered_map<char, NonTerminal>& non_terminals);

 public:
  void add_rule(char from, std::string_view to);

  Grammar get_grammar() const;
};
