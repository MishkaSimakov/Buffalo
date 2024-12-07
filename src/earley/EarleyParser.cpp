#include "EarleyParser.h"

#include <algorithm>

void EarleyParser::scan_action(const SituationsContainer& previous,
                               SituationsContainer& next, char symbol) {
  for (Situation situation : extract_situations(previous, symbol)) {
    ++situation.dot_position;
    emplace_situation(next, situation);
  }
}

EarleyParser EarleyParser::fit(Grammar grammar) {
  grammar.extend();
  EarleyParser parser(std::move(grammar));
  return parser;
}

bool EarleyParser::predict(std::string_view word) const {
  // TODO: empty word!!!

  size_t size = word.size();

  std::vector<SituationsContainer> situations(size + 1);

  // temporary containers for new situations
  SituationsContainer prev_added;
  SituationsContainer curr_added;

  Situation start(grammar_.get_start(),
                  grammar_.get_start_productions().front(), 0, 0);
  emplace_situation(curr_added, start);

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

      // complete
      for (const Situation& situation : extract_situations(prev_added, 0)) {
        auto parents = extract_situations(situations[situation.prev_position],
                                          situation.from.as_number());
        for (Situation parent_situation : parents) {
          ++parent_situation.dot_position;
          emplace_situation_if_new(situations[i], curr_added, parent_situation);
        }
      }

      // predict
      for (const auto& [non_terminal, productions] :
           grammar_.get_productions()) {
        if (!prev_added.contains(non_terminal.as_number())) {
          continue;
        }

        for (size_t production_id = 0; production_id < productions.size();
             ++production_id) {
          Situation new_situation(non_terminal, productions[production_id], i,
                                  production_id);

          emplace_situation_if_new(situations[i], curr_added, new_situation);
        }
      }
    }
  }

  return std::ranges::any_of(extract_situations(situations[size], 0),
                             [this](const Situation& situation) {
                               return situation.from == grammar_.get_start();
                             });
}
