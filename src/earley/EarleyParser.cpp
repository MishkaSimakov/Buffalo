#include "EarleyParser.h"

#include <algorithm>

void EarleyParser::scan_action(const SituationsContainer& previous,
                               SituationsContainer& next, char symbol) {
  for (Situation situation : extract_situations(previous, symbol)) {
    ++situation.dot_position;
    emplace_situation(next, situation);
  }
}

void EarleyParser::predict_action(
    const SituationsContainer& previously_added,
    SituationsContainer& currently_added,
    const std::vector<SituationsContainer>& all_situations,
    size_t index) const {
  for (const auto& [non_terminal, productions] : grammar_.get_productions()) {
    if (!previously_added.contains(non_terminal.as_number())) {
      continue;
    }

    for (size_t production_id = 0; production_id < productions.size();
         ++production_id) {
      Situation new_situation(non_terminal, productions[production_id], index,
                              production_id);

      emplace_situation_if_new(all_situations[index], currently_added,
                               new_situation);
    }
  }
}

void EarleyParser::complete_action(
    const SituationsContainer& previously_added,
    SituationsContainer& currently_added,
    const std::vector<SituationsContainer>& all_situations,
    size_t index) const {
  for (const Situation& situation : extract_situations(previously_added, 0)) {
    auto parents = extract_situations(all_situations[situation.prev_position],
                                      situation.from.as_number());
    for (Situation parent_situation : parents) {
      ++parent_situation.dot_position;
      emplace_situation_if_new(all_situations[index], currently_added,
                               parent_situation);
    }
  }
}

EarleyParser EarleyParser::fit(Grammar grammar) {
  grammar.optimize();
  EarleyParser parser(std::move(grammar));
  return parser;
}

bool EarleyParser::predict(std::string_view word) const {
  size_t size = word.size();

  std::vector<SituationsContainer> situations(size + 1);

  // temporary containers for new situations
  SituationsContainer prev_added;
  SituationsContainer curr_added;

  auto start_productions = grammar_.get_start_productions();
  for (size_t i = 0; i < start_productions.size(); ++i) {
    Situation start(grammar_.get_start(), start_productions[i], 0, i);
    emplace_situation(curr_added, start);
  }

  for (size_t i = 0; i <= size; ++i) {
    if (i != 0) {
      scan_action(situations[i - 1], curr_added, word[i - 1]);
    }

    while (!curr_added.empty()) {
      for (const auto& [symbol, added_situations] : curr_added) {
        for (const Situation& situation : added_situations) {
          situations[i][symbol].emplace(situation);
        }
      }

      std::swap(curr_added, prev_added);
      curr_added.clear();

      complete_action(prev_added, curr_added, situations, i);

      predict_action(prev_added, curr_added, situations, i);
    }
  }

  return std::ranges::any_of(extract_situations(situations[size], 0),
                             [this](const Situation& situation) {
                               return situation.from == grammar_.get_start();
                             });
}
