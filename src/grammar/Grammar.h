#pragma once

#include <functional>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "GrammarProduction.h"

class Grammar {
  // non-terminal id / productions with it
  std::unordered_map<NonTerminal, std::vector<GrammarProductionResult>>
      productions_;
  NonTerminal start_;

  void erase_unlisted(const std::unordered_set<NonTerminal>& keep);

  static std::list<GrammarProductionResult> generate_reduced_productions(
      const GrammarProductionResult& production,
      const std::unordered_set<NonTerminal>& epsilon_producing);

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

  // 1. remove non-producing non-terminals
  // 2. remove unreachable non-terminals
  // 3. remove epsilon-producing non-terminals
  void optimize();

  void remove_non_producing();
  void remove_unreachable();
  void remove_epsilon_producing();

  void add_rule(NonTerminal from, GrammarProductionResult to);
};
