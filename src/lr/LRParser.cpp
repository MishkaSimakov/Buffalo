#include "LRParser.h"

LRParser::LRParser(LRParserDetails::LRTableBuilder table_builder)
    : actions_(std::move(table_builder.get_actions_table())),
      goto_(std::move(table_builder.get_goto_table())) {}

LRParser LRParser::fit(Grammar grammar) {
  grammar.optimize();
  return LRParser(LRParserDetails::LRTableBuilder(grammar));
}

bool LRParser::predict(std::string_view word) const {
  // stores states indices
  std::vector<size_t> stack;

  stack.push_back(0);

  size_t index = 0;
  while (index <= word.size()) {
    char symbol = index < word.size() ? word[index] : cWordEndSymbol;

    Action action = actions_[stack.back()][symbol - 'a'];

    if (std::holds_alternative<LRParserDetails::AcceptAction>(action)) {
      return true;
    }
    if (std::holds_alternative<LRParserDetails::RejectAction>(action)) {
      return false;
    }
    if (std::holds_alternative<LRParserDetails::ShiftAction>(action)) {
      stack.push_back(
          std::get<LRParserDetails::ShiftAction>(action).next_state);
      ++index;
    } else {
      // action is reduce
      auto reduce = std::get<LRParserDetails::ReduceAction>(action);
      stack.resize(stack.size() - reduce.remove_count);
      stack.push_back(goto_[stack.back()].at(reduce.next.as_number()));
    }
  }

  throw std::runtime_error("Something went terribly wrong.");
}
