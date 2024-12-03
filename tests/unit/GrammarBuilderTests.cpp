#include <gtest/gtest.h>

#include "GrammarBuilder.h"

TEST(GrammarBuilderTests, test_start_symbol_is_correct) {
  GrammarBuilder builder;
  builder.add_rule('S', "helloworld");
  auto grammar = builder.get_grammar();

  auto start_rules = grammar.get_start_productions();
  ASSERT_EQ(start_rules.size(), 1);

  auto parts = start_rules.front().get_parts();
  ASSERT_EQ(parts.size(), 1);
  ASSERT_EQ(parts.front().index(), GrammarProductionResult::cTerminalIndex);
  ASSERT_STREQ(std::get<Terminal>(parts.front()).get_string().data(),
               "helloworld");
}

TEST(GrammarBuilderTests, test_simple_cases) {
  {
    GrammarBuilder builder;
    builder.add_rule('S', "");
    auto grammar = builder.get_grammar();

    auto start_rules = grammar.get_start_productions();
    ASSERT_EQ(start_rules.size(), 1);

    auto parts = start_rules.front().get_parts();
    ASSERT_TRUE(parts.empty());
  }

  {
    GrammarBuilder builder;
    builder.add_rule('S', "S");
    auto grammar = builder.get_grammar();

    auto start_rules = grammar.get_start_productions();
    ASSERT_EQ(start_rules.size(), 1);

    auto parts = start_rules.front().get_parts();
    ASSERT_EQ(parts.size(), 1);
    ASSERT_EQ(parts.front().index(),
              GrammarProductionResult::cNonTerminalIndex);
    ASSERT_EQ(std::get<NonTerminal>(parts.front()), grammar.get_start());
  }
}
