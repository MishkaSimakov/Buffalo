#pragma once
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "Parser.h"
#include "helpers/TupleHasher.h"

class EarleyParser {
  struct Situation {
    NonTerminal from;
    GrammarProductionResult::const_iterator dot_position;
    const GrammarProductionResult::const_iterator end_position;
    size_t prev_position;
    size_t production_index;

    Situation(NonTerminal left, const GrammarProductionResult& right,
              size_t prev_position, size_t production_index)
        : from(left),
          dot_position(right.cbegin()),
          end_position(right.cend()),
          prev_position(prev_position),
          production_index(production_index) {}

    bool operator==(const Situation& other) const = default;
  };

  struct SituationHasher {
    size_t operator()(const Situation& situation) const {
      return tuple_hasher_fn(situation.from, situation.dot_position,
                             situation.prev_position,
                             situation.production_index);
    }
  };

  using SituationsSet = std::unordered_set<Situation, SituationHasher>;
  using SituationsContainer = std::unordered_map<ssize_t, SituationsSet>;

  Grammar grammar_;

  explicit EarleyParser(Grammar grammar) : grammar_(std::move(grammar)) {}

  static void scan_action(const SituationsContainer& previous,
                          SituationsContainer& next, char symbol);

  void predict_action(const SituationsContainer& previously_added,
                       SituationsContainer& currently_added,
                       const std::vector<SituationsContainer>& all_situations,
                       size_t index) const;

  void complete_action(const SituationsContainer& previously_added,
                       SituationsContainer& currently_added,
                       const std::vector<SituationsContainer>& all_situations,
                       size_t index) const;

  static auto extract_situations(const SituationsContainer& container,
                                 ssize_t next_symbol) {
    auto itr = container.find(next_symbol);

    if (itr == container.end()) {
      return std::ranges::subrange<SituationsSet::iterator>{};
    }

    return std::ranges::subrange{itr->second.begin(), itr->second.end()};
  }

  static void emplace_situation(SituationsContainer& container,
                                Situation situation) {
    ssize_t next_symbol = situation.dot_position == situation.end_position
                              ? 0
                              : situation.dot_position.as_number();

    container[next_symbol].emplace(situation);
  }

  static void emplace_situation_if_new(const SituationsContainer& situations,
                                       SituationsContainer& destination,
                                       Situation situation) {
    ssize_t next_symbol = situation.dot_position == situation.end_position
                              ? 0
                              : situation.dot_position.as_number();

    auto itr = situations.find(next_symbol);
    if (itr != situations.end() && itr->second.contains(situation)) {
      return;
    }

    destination[next_symbol].emplace(situation);
  }

 public:
  static EarleyParser fit(Grammar grammar);

  bool predict(std::string_view word) const;
};

static_assert(IsParser<EarleyParser>);
