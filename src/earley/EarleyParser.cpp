#include "EarleyParser.h"

void EarleyParser::scan_action(const SituationsContainer& previous,
                               SituationsContainer& next, char symbol) {
  for (Situation situation : extract_situations(previous, symbol)) {
    char next_symbol = situation.shift();
    next.emplace(next_symbol, situation);
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

  Situation start{grammar_.get_rules_for(Grammar::cExtendedGrammarStart)[0], 0,
                  0};
  curr_added.emplace(Grammar::cGrammarStart, start);

  for (size_t i = 0; i <= size; ++i) {
    if (i != 0) {
      scan_action(situations[i - 1], curr_added, word[i - 1]);
    }

    while (!curr_added.empty()) {
      std::swap(curr_added, prev_added);
      curr_added.clear();

      // complete
      for (const Situation& situation :
           extract_situations(prev_added, Grammar::cRuleEndSymbol)) {
        auto parents = extract_situations(situations[situation.prev_position],
                                          situation.rule.left);
        for (Situation parent_situation : parents) {
          char next_symbol = parent_situation.shift();
          curr_added.emplace(next_symbol, parent_situation);
        }
      }

      // predict
      for (char non_terminal : Grammar::get_non_terminals_view()) {
        if (!prev_added.contains(non_terminal)) {
          continue;
        }

        for (const Rule& rule : grammar_.get_rules_for(non_terminal)) {
          char next_symbol = rule.right.front();
          Situation new_situation{rule, 0, i};
          curr_added.emplace(next_symbol, new_situation);
        }
      }

      for (const auto& [symbol, situation] : curr_added) {
        situations[i].emplace(symbol, situation);
      }
    }
  }

  for (const Situation& situation :
       extract_situations(situations[size], Grammar::cRuleEndSymbol)) {
    if (situation.prev_position == 0 && situation.dot_position == 1 &&
        situation.rule.left == Grammar::cExtendedGrammarStart) {
      return true;
    }
  }

  return false;
}
