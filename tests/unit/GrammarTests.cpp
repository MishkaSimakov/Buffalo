#include <gtest/gtest.h>

#include "Grammar.h"

const static std::pair<Grammar, std::vector<NonTerminal>> test_grammar = [] {
  Grammar grammar;
  NonTerminal start;
  NonTerminal the_void;
  NonTerminal slan;
  NonTerminal ubik;
  NonTerminal conrad;
  NonTerminal femto;

  // start -> the_void + slan -> "bca" + "abc"
  // third, fourth, and fifth are non producing
  grammar.add_rule(start, the_void + slan);
  grammar.add_rule(slan, slan + slan);
  grammar.add_rule(slan, start);
  grammar.add_rule(start, start);
  grammar.add_rule(slan, Terminal{"abc"});
  grammar.add_rule(the_void, Terminal{"bca"});

  grammar.add_rule(start, femto);
  grammar.add_rule(ubik, ubik + the_void);
  grammar.add_rule(ubik, conrad);

  grammar.set_start(start);

  return std::make_pair(
      grammar, std::vector{start, the_void, slan, ubik, conrad, femto});
}();

TEST(GrammarTests, test_it_stores_start_non_terminal) {
  Grammar grammar;

  NonTerminal start;
  NonTerminal first;
  NonTerminal second;

  grammar.add_rule(start, Terminal{"abc"});
  grammar.add_rule(first, start + second);
  grammar.add_rule(second, start + second);

  grammar.set_start(start);

  ASSERT_EQ(grammar.get_start(), start);

  auto productions = grammar.get_start_productions();
  ASSERT_EQ(productions.size(), 1);

  const auto& parts = productions.front().get_parts();
  ASSERT_EQ(parts.size(), 1);
  ASSERT_TRUE(std::holds_alternative<Terminal>(parts.front()));
  ASSERT_STREQ(std::get<Terminal>(parts.front()).get_string().data(), "abc");
}

TEST(GrammarTests, test_it_removes_non_producing_correctly) {
  auto [grammar, non_terminals] = test_grammar;
  grammar.remove_non_producing();

  std::vector producing = {non_terminals[0], non_terminals[1],
                           non_terminals[2]};
  ASSERT_EQ(grammar.get_productions().size(), producing.size());

  for (NonTerminal non_terminal : producing) {
    ASSERT_TRUE(grammar.get_productions().contains(non_terminal));
  }
}

TEST(GrammarTests, test_it_removes_unreachable_correctly) {
  auto [grammar, non_terminals] = test_grammar;

  grammar.remove_unreachable();

  std::vector reachable = {non_terminals[0], non_terminals[1], non_terminals[2],
                           non_terminals[5]};
  ASSERT_EQ(grammar.get_productions().size(), reachable.size());

  for (NonTerminal non_terminal : reachable) {
    ASSERT_TRUE(grammar.get_productions().contains(non_terminal));
  }
}

// TODO: add tests for epsilon producing and for empty grammars