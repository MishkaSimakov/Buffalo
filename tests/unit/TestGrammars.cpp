#include "TestGrammars.h"

#include "GrammarBuilder.h"

static Grammar GetEvenPalindromesGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSa");
  builder.add_rule('S', "bSb");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

static Grammar GetPalindromesGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSa");
  builder.add_rule('S', "bSb");
  builder.add_rule('S', "b");
  builder.add_rule('S', "a");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

static Grammar GetParenthesesGrammar() {
  GrammarBuilder builder;

  // here a is (, b is )
  builder.add_rule('S', "SS");
  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
}

static Grammar GetMatchingPairsGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
}

static Grammar GetMatchingPairsWithEmptyGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

static Grammar GetEmptyGrammar() { return {}; }

static Grammar GetEmptyWordGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "");

  return builder.get_grammar();
}

static Grammar GetSophisticatedEmptyWordGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "ABCD");
  builder.add_rule('A', "S");
  builder.add_rule('B', "A");
  builder.add_rule('D', "F");
  builder.add_rule('F', "");
  builder.add_rule('K', "abc");
  builder.add_rule('C', "CA");
  builder.add_rule('C', "");

  return builder.get_grammar();
}

const auto test_grammars = [] {
  // name / grammar / acceptable / unacceptable
  std::vector<TestGrammarT> tests;

  // clang-format off
  tests.emplace_back(
    "even palindromes",
    GetEvenPalindromesGrammar(),
    std::vector{"", "aa", "bb", "abba", "baab", "bbbb", "aaaa", "aabbabbabbaa"},
    std::vector{"a", "b", "ab", "ba", "aba", "abab", "bab", "aabbabbabbab", "babbabbabbaa", "c", "ac", "aac"}
  );

  tests.emplace_back(
    "palindromes palindromes",
    GetPalindromesGrammar(),
    std::vector{"", "a", "b", "aa", "bb", "aba", "aaa", "bbb", "bab", "abba", "baab", "bbbb", "aaaa", "aabbabbabbaa"},
    std::vector{"ab", "ba", "baa", "aab", "bba", "abb", "abab", "aabbabbabbab", "babbabbabbaa", "c", "ac", "aac", "aca"}
  );

  tests.emplace_back(
    "parentheses",
    GetParenthesesGrammar(),
    std::vector{"ab", "abab", "aabb", "abaabb", "aababb", "aaabbabb", "aaabbabbaaabbabbaaabbabb"},
    std::vector{"", "a", "b", "aababbbabb", "aba", "bab"}
  );

  tests.emplace_back(
    "matching pairs (a^n b^n, n > 0)",
    GetMatchingPairsGrammar(),
    std::vector{"ab", "aabb", "aaabbb", "aaaabbbb", "aaaaaaaaaaaaabbbbbbbbbbbbb"},
    std::vector{"", "a", "b", "aa", "bb", "abb", "aab", "aaaaaaaaaaaaabbbbbbbbbbbb", "aaaaaaaaaaaabbbbbbbbbbbbb"}
  );

  tests.emplace_back(
    "matching pairs (a^n b^n, n >= 0)",
    GetMatchingPairsWithEmptyGrammar(),
    std::vector{"", "ab", "aabb", "aaabbb", "aaaabbbb", "aaaaaaaaaaaaabbbbbbbbbbbbb"},
    std::vector{"a", "b", "aa", "bb", "abb", "aab", "aaaaaaaaaaaaabbbbbbbbbbbb", "aaaaaaaaaaaabbbbbbbbbbbbb"}
  );

  tests.emplace_back(
    "nothing",
    GetEmptyGrammar(),
    std::vector<const char*>{},
    std::vector{"", "a", "b", "ab", "aa", "ba", "bb", "aaabbbbaababaababsdff", "d"}
  );

  tests.emplace_back(
    "only empty word",
    GetEmptyWordGrammar(),
    std::vector{""},
    std::vector{"a", "b", "ab", "aa", "ba", "bb", "aaabbbbaababaababsdff", "d"}
  );

  tests.emplace_back(
    "only empty word (sophisticated)",
    GetSophisticatedEmptyWordGrammar(),
    std::vector{""},
    std::vector{"a", "b", "ab", "aa", "ba", "bb", "abc", "aaabbbbaababaababsdff", "d"}
  );

  // clang-format on

  return tests;
}();
