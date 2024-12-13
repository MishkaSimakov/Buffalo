#include "Grammar.h"

#include <unordered_set>

void Grammar::erase_unlisted(const std::unordered_set<NonTerminal>& keep) {
  std::erase_if(productions_, [&keep](const auto& pair) {
    return !keep.contains(pair.first);
  });

  // we could have removed start non-terminal so we must add new start
  if (!keep.contains(start_)) {
    NonTerminal start;
    set_start(start);
  }
}

void Grammar::remove_non_producing() {
  using PartT = GrammarProductionResult::PartT;

  std::unordered_set<NonTerminal> producing;
  std::unordered_set<NonTerminal> to_process;

  for (const auto& [non_terminal, productions] : productions_) {
    for (const GrammarProductionResult& production : productions) {
      if (production.is_terminal()) {
        to_process.emplace(non_terminal);
        break;
      }
    }
  }

  while (!to_process.empty()) {
    producing.merge(to_process);
    to_process.clear();

    for (const auto& [non_terminal, productions] : productions_) {
      for (const auto& production : productions) {
        bool is_producing = std::ranges::all_of(
            production.get_parts(), [&producing](const PartT& part) {
              if (std::holds_alternative<Terminal>(part)) {
                return true;
              }

              return producing.contains(std::get<NonTerminal>(part));
            });

        if (is_producing && !producing.contains(non_terminal)) {
          to_process.emplace(non_terminal);
          break;
        }
      }
    }
  }

  erase_unlisted(producing);
}

void Grammar::remove_unreachable() {
  using PartT = GrammarProductionResult::PartT;

  std::unordered_set<NonTerminal> reachable;
  std::unordered_set<NonTerminal> to_process;

  to_process.emplace(start_);

  while (!to_process.empty()) {
    auto prev_updated = to_process;
    reachable.merge(to_process);
    to_process.clear();

    for (NonTerminal non_terminal : prev_updated) {
      for (const auto& production : productions_[non_terminal]) {
        for (const PartT& part : production.get_parts()) {
          if (std::holds_alternative<Terminal>(part)) {
            continue;
          }

          NonTerminal next = std::get<NonTerminal>(part);
          if (!reachable.contains(next)) {
            to_process.emplace(next);
          }
        }
      }
    }
  }

  erase_unlisted(reachable);
}

void Grammar::remove_epsilon_producing() {
  using PartT = GrammarProductionResult::PartT;

  // find epsilon producing non-terminals
  std::unordered_set<NonTerminal> epsilon_producing;
  std::unordered_set<NonTerminal> to_process;

  for (const auto& [non_terminal, productions] : productions_) {
    for (const auto& production : productions) {
      if (production.is_empty()) {
        to_process.emplace(non_terminal);
        break;
      }
    }
  }

  while (!to_process.empty()) {
    epsilon_producing.merge(to_process);
    to_process.clear();

    for (const auto& [non_terminal, productions] : productions_) {
      for (const auto& production : productions) {
        bool is_epsilon_producing = std::ranges::all_of(
            production.get_parts(), [&epsilon_producing](const PartT& part) {
              if (std::holds_alternative<Terminal>(part)) {
                return std::get<Terminal>(part).get_string().empty();
              }

              return epsilon_producing.contains(std::get<NonTerminal>(part));
            });

        if (is_epsilon_producing && !epsilon_producing.contains(non_terminal)) {
          to_process.emplace(non_terminal);
          break;
        }
      }
    }
  }

  // add neccessary rules to grammar
  for (const auto& [non_terminal, productions] : productions_) {
    std::list<GrammarProductionResult> new_productions;

    for (const auto& production : productions) {
      auto reduced =
          generate_reduced_productions(production, epsilon_producing);
      new_productions.splice(new_productions.end(), reduced);
    }

    productions_[non_terminal].clear();
    for (auto& production : new_productions) {
      productions_[non_terminal].emplace_back(std::move(production));
    }
  }

  // remove all rules A -> epsilon
  for (auto& productions : productions_ | std::views::values) {
    std::erase_if(productions, [](const GrammarProductionResult& production) {
      return production.is_empty();
    });
  }

  // add S' -> S and potentially S' -> epsilon
  NonTerminal new_start;
  add_rule(new_start, start_);

  if (epsilon_producing.contains(start_)) {
    add_rule(new_start, GrammarProductionResult::empty());
  }

  set_start(new_start);
}
std::list<GrammarProductionResult> Grammar::generate_reduced_productions(
    const GrammarProductionResult& production,
    const std::unordered_set<NonTerminal>& epsilon_producing) {
  using PartT = GrammarProductionResult::PartT;
  std::list result{GrammarProductionResult::empty()};

  for (const PartT& part : production.get_parts()) {
    if (std::holds_alternative<Terminal>(part) ||
        !epsilon_producing.contains(std::get<NonTerminal>(part))) {
      for (GrammarProductionResult& result_production : result) {
        result_production.add_part(part);
      }
        } else {
          auto copy = result;
          for (auto& result_production : result) {
            result_production.add_part(part);
          }
          result.splice(result.end(), copy);
        }
  }

  return result;
}

void Grammar::optimize() {
  remove_non_producing();
  remove_unreachable();
  remove_epsilon_producing();
}

void Grammar::add_rule(NonTerminal from, GrammarProductionResult to) {
  productions_[from].push_back(std::move(to));
}
