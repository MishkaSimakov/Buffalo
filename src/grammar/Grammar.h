#pragma once

#include <ranges>
#include <unordered_map>
#include <vector>

#include "GrammarProduction.h"

class Grammar {
  // non-terminal id / productions with it
  std::unordered_map<NonTerminal, std::vector<GrammarProductionResult>>
      productions_;
  NonTerminal start_;

 public:
  const auto& get_productions() const { return productions_; }

  std::span<const GrammarProductionResult> get_productions_for(
      const NonTerminal& non_terminal) const {
    auto itr = productions_.find(non_terminal);

    if (itr == productions_.end()) {
      return {};
    }

    return itr->second;
  }

  NonTerminal get_start() const { return start_; }
  void set_start(NonTerminal new_start) { start_ = new_start; }
  auto get_start_productions() const { return get_productions_for(start_); }

  void extend();
  void remove_epsilon_rules();
  void remove_unreachable();

  void add_rule(NonTerminal from, GrammarProductionResult to);
};
