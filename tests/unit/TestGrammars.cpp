#include "TestGrammars.h"

static Grammar GetEvenPalindromesGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "aSa");
  grammar.add_rule('S', "bSb");
  grammar.add_rule('S', "");

  return grammar;
}

static Grammar GetPalindromesGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "aSa");
  grammar.add_rule('S', "bSb");
  grammar.add_rule('S', "b");
  grammar.add_rule('S', "a");
  grammar.add_rule('S', "");

  return grammar;
}

static Grammar GetParenthesesGrammar() {
  Grammar grammar;

  // here a is (, b is )
  grammar.add_rule('S', "SS");
  grammar.add_rule('S', "aSb");
  grammar.add_rule('S', "ab");

  return grammar;
}

static Grammar GetMatchingPairsGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "aSb");
  grammar.add_rule('S', "ab");

  return grammar;
}

static Grammar GetMatchingPairsWithEmptyGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "aSb");
  grammar.add_rule('S', "ab");
  grammar.add_rule('S', "");

  return grammar;
}

static Grammar GetEmptyGrammar() { return {}; }

static Grammar GetEmptyWordGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "");

  return grammar;
}

static Grammar GetSophisticatedEmptyWordGrammar() {
  Grammar grammar;

  grammar.add_rule('S', "ABCD");
  grammar.add_rule('A', "S");
  grammar.add_rule('B', "A");
  grammar.add_rule('D', "F");
  grammar.add_rule('F', "");
  grammar.add_rule('K', "abc");
  grammar.add_rule('C', "CA");
  grammar.add_rule('C', "");

  return grammar;
}

const auto test_grammars = [] {
  // name / grammar / acceptable / unacceptable
  std::vector<std::tuple<const char*, Grammar, std::vector<const char*>,
                         std::vector<const char*>>>
      tests;

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
