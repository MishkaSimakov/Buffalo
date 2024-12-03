#pragma once
#include <ranges>
#include <unordered_map>

#include "Parser.h"

class EarleyParser {
  struct Situation {
    NonTerminal from;
    GrammarProductionResult::const_iterator dot_position;
    const GrammarProductionResult::const_iterator end_position;
    size_t prev_position;

    Situation(NonTerminal left, const GrammarProductionResult& right,
              size_t prev_position)
        : from(left),
          dot_position(right.cbegin()),
          end_position(right.cend()),
          prev_position(prev_position) {}
  };

  using SituationsContainer = std::unordered_multimap<ssize_t, Situation>;

  Grammar grammar_;

  explicit EarleyParser(Grammar grammar) : grammar_(std::move(grammar)) {}

  static void scan_action(const SituationsContainer& previous,
                          SituationsContainer& next, char symbol);

  static auto extract_situations(const SituationsContainer& container,
                                 ssize_t next_symbol) {
    auto [beg, end] = container.equal_range(next_symbol);
    return std::ranges::subrange{beg, end} | std::views::values;
  }

  static auto emplace_situation(SituationsContainer& container,
                                Situation situation) {
    if (situation.dot_position != situation.end_position) {
      container.emplace(situation.dot_position.as_number(), situation);
    } else {
      container.emplace(0, situation);
    }
  }

 public:
  static EarleyParser fit(Grammar grammar);

  bool predict(std::string_view word) const;
};

static_assert(IsParser<EarleyParser>);
