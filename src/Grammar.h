#pragma once

#include <array>
#include <ranges>
#include <string>
#include <vector>

struct Rule {
  char left;
  std::string right;
};

class Grammar {
  // +1 for @ for extended grammar
  static constexpr size_t cNonTerminalsCount = 'Z' - 'A' + 2;

  std::array<std::vector<Rule>, cNonTerminalsCount> rules_;



 public:
  static constexpr char cExtendedGrammarStart = '@';
  static constexpr char cGrammarStart = 'S';
  static constexpr char cRuleEndSymbol = '$';

  static bool is_acceptable_terminal(char symbol) {
    return 'a' <= symbol && symbol <= 'z';
  }

  static bool is_acceptable_non_terminal(char symbol) {
    return 'A' <= symbol && symbol <= 'Z';
  }

  const std::vector<Rule>& get_rules_for(char non_terminal) const {
    return rules_[non_terminal - cExtendedGrammarStart];
  }

  void extend();
  void remove_epsilon_rules();
  void remove_unreachable();

  static auto get_non_terminals_view() {
    return std::ranges::iota_view{'A', static_cast<char>('Z' + 1)};
  }

  void add_rule(char from, std::string_view to);
};
