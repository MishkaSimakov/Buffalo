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
                  grammar_.get_start_productions().front(), 0);
  emplace_situation(curr_added, start);

  for (size_t i = 0; i <= size; ++i) {
    if (i != 0) {
      scan_action(situations[i - 1], curr_added, word[i - 1]);
    }

    while (!curr_added.empty()) {
      std::swap(curr_added, prev_added);
      curr_added.clear();

      // complete
      for (const Situation& situation : extract_situations(prev_added, 0)) {
        auto parents = extract_situations(situations[situation.prev_position],
                                          situation.from.as_number());
        for (Situation parent_situation : parents) {
          ++parent_situation.dot_position;
          emplace_situation(curr_added, parent_situation);
        }
      }

      // predict
      for (NonTerminal non_terminal :
           grammar_.get_productions() | std::views::keys) {
        if (!prev_added.contains(non_terminal.as_number())) {
          continue;
        }

        for (const GrammarProductionResult& production :
             grammar_.get_productions_for(non_terminal)) {
          Situation new_situation(non_terminal, production, i);
          emplace_situation(curr_added, new_situation);
        }
      }

      for (const auto& [symbol, situation] : curr_added) {
        situations[i].emplace(symbol, situation);
      }
    }
  }

  return std::ranges::any_of(extract_situations(situations[size], 0),
                             [this](const Situation& situation) {
                               return situation.from == grammar_.get_start();
                             });
}
