#include <gtest/gtest.h>

#include "Grammar.h"
#include "LRTableBuilder.h"

std::unordered_set<char> MakeSet(std::initializer_list<char> symbols) {
  return {symbols};
}

constexpr char cWordEndSymbol = LRParserDetails::LRTableBuilder::cWordEndSymbol;

TEST(LRTableBuilderTests, test_first_and_follow_tables_is_correct) {
  {
    Grammar grammar;
    NonTerminal start;
    NonTerminal another;

    grammar.add_rule(start, another);
    grammar.add_rule(another, Terminal{"abc"});
    grammar.add_rule(another, Terminal{"cba"});

    grammar.set_start(start);

    auto builder = LRParserDetails::LRTableBuilder(grammar);
    const auto& first = builder.get_first_table();
    const auto& follow = builder.get_follow_table();

    ASSERT_EQ(first.at(start), MakeSet({'a', 'c'}));
    ASSERT_EQ(first.at(another), MakeSet({'a', 'c'}));

    ASSERT_EQ(follow.at(start), MakeSet({cWordEndSymbol}));
    ASSERT_EQ(follow.at(another), MakeSet({cWordEndSymbol}));
  }

  {
    Grammar grammar;
    NonTerminal start;
    NonTerminal left;
    NonTerminal middle;
    NonTerminal right;

    grammar.add_rule(start, left + Terminal{"s"} + middle + right);
    grammar.add_rule(start, Terminal{"s"} + middle + right);
    grammar.add_rule(left, Terminal{"a"});
    grammar.add_rule(middle, middle + Terminal{"a"});
    grammar.add_rule(middle, Terminal{"b"});
    grammar.add_rule(right, Terminal{"c"});

    grammar.set_start(start);

    auto builder = LRParserDetails::LRTableBuilder(grammar);
    const auto& first = builder.get_first_table();
    const auto& follow = builder.get_follow_table();

    ASSERT_EQ(first.at(start), MakeSet({'a', 's'}));
    ASSERT_EQ(first.at(left), MakeSet({'a'}));
    ASSERT_EQ(first.at(middle), MakeSet({'b'}));
    ASSERT_EQ(first.at(right), MakeSet({'c'}));

    ASSERT_EQ(follow.at(start), MakeSet({cWordEndSymbol}));
    ASSERT_EQ(follow.at(left), MakeSet({'s'}));
    ASSERT_EQ(follow.at(middle), MakeSet({'a', 'c'}));
    ASSERT_EQ(follow.at(right), MakeSet({cWordEndSymbol}));
  }

  {
    Grammar grammar;
    NonTerminal start;
    NonTerminal add;
    NonTerminal factor;
    NonTerminal term;

    grammar.add_rule(start, add);
    grammar.add_rule(add, add + Terminal{"a"} + factor);
    grammar.add_rule(add, factor);

    grammar.add_rule(factor, factor + Terminal{"b"} + term);
    grammar.add_rule(factor, term);

    grammar.add_rule(term, Terminal{"c"} + add + Terminal{"d"});
    grammar.add_rule(term, Terminal{"hello"});

    grammar.set_start(start);

    auto builder = LRParserDetails::LRTableBuilder(grammar);
    const auto& first = builder.get_first_table();
    const auto& follow = builder.get_follow_table();

    ASSERT_EQ(first.at(start), MakeSet({'c', 'h'}));
    ASSERT_EQ(first.at(add), MakeSet({'c', 'h'}));
    ASSERT_EQ(first.at(factor), MakeSet({'c', 'h'}));
    ASSERT_EQ(first.at(term), MakeSet({'c', 'h'}));

    ASSERT_EQ(follow.at(start), MakeSet({cWordEndSymbol}));
    ASSERT_EQ(follow.at(add), MakeSet({'a', 'd', cWordEndSymbol}));
    ASSERT_EQ(follow.at(factor), MakeSet({'a', 'b', 'd', cWordEndSymbol}));
    ASSERT_EQ(follow.at(term), MakeSet({'a', 'b', 'd', cWordEndSymbol}));
  }
}
