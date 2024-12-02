#pragma once
#include <ranges>
#include <unordered_map>

#include "Parser.h"

class EarleyParser {
  struct Situation {
    const Rule& rule;
    size_t dot_position;
    size_t prev_position;

    char shift() {
      ++dot_position;
      return rule.right[dot_position];
    }
  };

  using SituationsContainer = std::unordered_multimap<char, Situation>;

  Grammar grammar_;

  explicit EarleyParser(Grammar grammar) : grammar_(std::move(grammar)) {}

  static void scan_action(const SituationsContainer& previous,
                          SituationsContainer& next, char symbol);

  static auto extract_situations(const SituationsContainer& container,
                                 char next_symbol) {
    auto [beg, end] = container.equal_range(next_symbol);
    return std::ranges::subrange{beg, end} | std::views::values;
  }

 public:
  static EarleyParser fit(Grammar grammar);

  bool predict(std::string_view word) const;
};

static_assert(IsParser<EarleyParser>);
