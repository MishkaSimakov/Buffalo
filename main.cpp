#include <iostream>

#include "Grammar.h"
#include "GrammarBuilder.h"
#include "LRParser.h"

namespace Strings {
const char* productions_separator = "->";
const char* invalid_production_syntax =
    "Every production must look like X -> seq, where X is one non-terminal "
    "and seq is sequence of terminals and non-termianls.";

const char* invalid_production_left_part =
    "In the left part of production must be a valid non-terminal.";
const char* invalid_production_right_part =
    "In the right part of production must be a sequence of valid "
    "terminals and non-terminals.";

const char* invalid_start_non_terminal =
    "Start non-terminal must be a valid non-terminal.";
}  // namespace Strings

std::pair<char, std::string> ParseProduction(std::string production) {
  std::erase_if(production,
                [](char symbol) { return std::isspace(symbol) != 0; });

  // separate string by ->
  size_t position = production.find(Strings::productions_separator);
  if (position != 1) {
    throw std::runtime_error(Strings::invalid_production_syntax);
  }

  char non_terminal = production[0];
  std::string production_result = production.substr(3);

  return {non_terminal, std::move(production_result)};
}

Grammar ReadGrammar() {
  size_t non_terminals_count;
  size_t terminals_count;
  size_t productions_count;

  std::cin >> non_terminals_count >> terminals_count >> productions_count;

  std::unordered_set<char> non_terminals;
  for (size_t i = 0; i < non_terminals_count; ++i) {
    char symbol;
    std::cin >> symbol;

    non_terminals.emplace(symbol);
  }

  std::unordered_set<char> terminals;
  for (size_t i = 0; i < terminals_count; ++i) {
    char symbol;
    std::cin >> symbol;

    terminals.emplace(symbol);
  }

  std::string production;

  // skip current line
  std::getline(std::cin, production);

  GrammarBuilder builder;
  for (size_t i = 0; i < productions_count; ++i) {
    std::getline(std::cin, production);

    auto [left, right] = ParseProduction(std::move(production));

    if (!non_terminals.contains(left)) {
      throw std::runtime_error(Strings::invalid_production_left_part);
    }

    bool is_right_valid =
        std::ranges::all_of(right, [&non_terminals, &terminals](char symbol) {
          return terminals.contains(symbol) || non_terminals.contains(symbol);
        });

    if (!is_right_valid) {
      throw std::runtime_error(Strings::invalid_production_right_part);
    }

    builder.add_rule(left, right);
  }

  char start_non_terminal;
  std::cin >> start_non_terminal;

  if (!non_terminals.contains(start_non_terminal)) {
    throw std::runtime_error(Strings::invalid_start_non_terminal);
  }

  return builder.get_grammar(start_non_terminal);
}

Grammar GetAlternativeParenthesesGrammar() {
  GrammarBuilder builder;

  // here a is (, b is )
  builder.add_rule('S', "aSbS");
  builder.add_rule('S', "aSb");
  builder.add_rule('S', "abS");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
}

int main() {
  auto grammar = ReadGrammar();
  auto parser = LRParser::fit(std::move(grammar));

  size_t words_count;
  std::cin >> words_count;

  std::string word;
  // skip current line
  std::getline(std::cin, word);

  for (size_t i = 0; i < words_count; ++i) {
    std::getline(std::cin, word);

    bool correct = parser.predict(std::move(word));
    std::cout << (correct ? "Yes" : "No") << std::endl;
  }
}
