#include "GrammarBuilder.h"

bool GrammarBuilder::is_valid_terminal(char symbol) {
  return 'a' <= symbol && symbol <= 'z';
}

bool GrammarBuilder::is_valid_non_terminal(char symbol) {
  return 'A' <= symbol && symbol <= 'Z';
}

GrammarProductionResult GrammarBuilder::string_to_production(
    std::string_view string,
    std::unordered_map<char, NonTerminal>& non_terminals) {
  GrammarProductionResult result;

  const char* terminal_begin = string.begin();
  const char* terminal_end = string.begin();

  for (; terminal_end != string.end(); ++terminal_end) {
    if (is_valid_terminal(*terminal_end)) {
      continue;
    }

    // *terminal_end is non-terminal
    if (terminal_end - terminal_begin > 0) {
      result += Terminal{{terminal_begin, terminal_end}};
    }

    terminal_begin = terminal_end + 1;
    result += non_terminals[*terminal_end];
  }

  if (terminal_end - terminal_begin > 0) {
    result += Terminal{{terminal_begin, terminal_end}};
  }

  return result;
}

void GrammarBuilder::add_rule(char from, std::string_view to) {
  // check rule validity
  if (!is_valid_non_terminal(from)) {
    throw std::runtime_error(
        "Left part of production must be non-terminal (in A-Z)");
  }

  for (char symbol : to) {
    if (!is_valid_non_terminal(symbol) && !is_valid_terminal(symbol)) {
      throw std::runtime_error(
          "Right part of production must contain only terminals (a-z) and "
          "non-terminals (A-Z)");
    }
  }

  productions_.emplace_back(from, to);
}

Grammar GrammarBuilder::get_grammar() const {
  Grammar result;
  std::unordered_map<char, NonTerminal> non_terminals;

  for (auto [from, to] : productions_) {
    auto production_result = string_to_production(to, non_terminals);
    result.add_rule(non_terminals[from], production_result);
  }

  result.set_start(non_terminals[cStartNonTerminal]);

  return result;
}
