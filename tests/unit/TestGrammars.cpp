#include "TestGrammars.h"

#include "GrammarBuilder.h"

namespace {
Grammar GetEvenPalindromesGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSa");
  builder.add_rule('S', "bSb");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

Grammar GetPalindromesGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSa");
  builder.add_rule('S', "bSb");
  builder.add_rule('S', "b");
  builder.add_rule('S', "a");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

Grammar GetParenthesesGrammar() {
  GrammarBuilder builder;

  // here a is (, b is )
  builder.add_rule('S', "SS");
  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
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

Grammar GetMatchingPairsGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
}

Grammar GetMatchingPairsWithEmptyGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

Grammar GetEvenCountOfBGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "AB");
  builder.add_rule('A', "SB");
  builder.add_rule('A', "B");
  builder.add_rule('B', "b");

  return builder.get_grammar();
}

Grammar GetEmptyGrammar() { return {}; }

Grammar GetEmptyWordGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "");

  return builder.get_grammar();
}

Grammar GetSophisticatedEmptyWordGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "ABCD");
  builder.add_rule('A', "S");
  builder.add_rule('A', "");
  builder.add_rule('B', "A");
  builder.add_rule('D', "F");
  builder.add_rule('F', "");
  builder.add_rule('K', "abc");
  builder.add_rule('C', "CA");
  builder.add_rule('C', "");

  return builder.get_grammar();
}

Grammar GetAkhcheckGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSbS");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

Grammar GetBuffaloGrammar() {
  Grammar grammar;

  NonTerminal start;
  grammar.set_start(start);

  grammar.add_rule(start, start + Terminal{" "} + start);
  grammar.add_rule(start, Terminal{"buffalo"});

  return grammar;
}

Grammar GetLRBuffaloGrammar() {
  Grammar grammar;

  NonTerminal start;
  NonTerminal sentence;
  NonTerminal buffalo;
  grammar.set_start(start);

  grammar.add_rule(start, sentence);
  grammar.add_rule(start, buffalo);
  grammar.add_rule(sentence, start + Terminal{" "} + buffalo);
  grammar.add_rule(buffalo, Terminal{"buffalo"});

  return grammar;
}

Grammar GetLoremIpsumGrammar() {
  Grammar grammar;

  NonTerminal text;
  NonTerminal sentence_with_dot;
  NonTerminal sentence;
  NonTerminal word;

  grammar.set_start(text);

  grammar.add_rule(text, text + Terminal{" "} + sentence_with_dot);
  grammar.add_rule(text, sentence_with_dot);

  grammar.add_rule(sentence_with_dot, sentence + Terminal{"."});

  grammar.add_rule(sentence, sentence + Terminal{" "} + word);
  grammar.add_rule(sentence, word);

  std::vector words = {
      "ad",         "sed",          "consectetur", "duis",
      "esse",       "deserunt",     "occaecat",    "dolore",
      "incididunt", "enim",         "ex",          "cupidatat",
      "commodo",    "consequat",    "pariatur",    "proident",
      "anim",       "aute",         "labore",      "excepteur",
      "veniam",     "nisi",         "amet",        "eu",
      "do",         "culpa",        "quis",        "cillum",
      "laboris",    "magna",        "lorem",       "ea",
      "ipsum",      "minim",        "irure",       "reprehenderit",
      "fugiat",     "exercitation", "velit",       "officia",
      "adipiscing", "dolor",        "et",          "sit",
      "nostrud",    "qui",          "mollit",      "voluptate",
      "ut",         "ullamco",      "sint",        "non",
      "id",         "est",          "aliquip",     "laborum",
      "nulla",      "tempor",       "elit",        "eiusmod",
      "aliqua",     "sunt",         "in"};

  for (const char* lorem_word : words) {
    grammar.add_rule(word, Terminal{lorem_word});
  }

  return grammar;
}

Grammar GetGrammarWithEpsilonProducing() {
  Grammar grammar;

  NonTerminal start;
  NonTerminal a_or_nothing;
  NonTerminal b;
  NonTerminal c_or_nothing;
  NonTerminal d;
  NonTerminal e_or_nothing;
  NonTerminal e;
  NonTerminal nothing;

  grammar.set_start(start);

  grammar.add_rule(start, a_or_nothing + b + c_or_nothing + d + e_or_nothing);

  grammar.add_rule(a_or_nothing, Terminal{"a"});
  grammar.add_rule(a_or_nothing, GrammarProductionResult::empty());

  grammar.add_rule(b, Terminal{"b"});

  grammar.add_rule(c_or_nothing, Terminal{"c"});
  grammar.add_rule(c_or_nothing, GrammarProductionResult::empty());

  grammar.add_rule(d, Terminal{"d"});

  grammar.add_rule(e_or_nothing, e);
  grammar.add_rule(e_or_nothing, nothing);

  grammar.add_rule(e, Terminal{"e"});
  grammar.add_rule(nothing, GrammarProductionResult::empty());

  return grammar;
}
}  // namespace

const auto test_grammars = [] {
  // name / grammar / acceptable / unacceptable / is lr(1)
  std::vector<TestGrammarT> tests;

  // clang-format off
  tests.emplace_back(
    "even palindromes",
    GetEvenPalindromesGrammar(),
    std::vector{"", "aa", "bb", "abba", "baab", "bbbb", "aaaa", "aabbabbabbaa"},
    std::vector{"a", "b", "ab", "ba", "aba", "abab", "bab", "aabbabbabbab", "babbabbabbaa", "c", "ac", "aac"},
    false
  );

  tests.emplace_back(
    "palindromes",
    GetPalindromesGrammar(),
    std::vector{"", "a", "b", "aa", "bb", "aba", "aaa", "bbb", "bab", "abba", "baab", "bbbb", "aaaa", "aabbabbabbaa"},
    std::vector{"ab", "ba", "baa", "aab", "bba", "abb", "abab", "aabbabbabbab", "babbabbabbaa", "c", "ac", "aac", "aca"},
    false
  );

  tests.emplace_back(
    "parentheses",
    GetParenthesesGrammar(),
    std::vector{"ab", "abab", "aabb", "abaabb", "aababb", "aaabbabb", "aaabbabbaaabbabbaaabbabb"},
    std::vector{"", "a", "b", "aababbbabb", "aba", "bab"},
    false
  );

  tests.emplace_back(
    "alternative parentheses",
    GetAlternativeParenthesesGrammar(),
    std::vector{"ab", "abab", "aabb", "abaabb", "aababb", "aaabbabb", "aaabbabbaaabbabbaaabbabb"},
    std::vector{"", "a", "b", "aababbbabb", "aba", "bab"},
    true
  );

  tests.emplace_back(
    "matching pairs (a^n b^n, n > 0)",
    GetMatchingPairsGrammar(),
    std::vector{"ab", "aabb", "aaabbb", "aaaabbbb", "aaaaaaaaaaaaabbbbbbbbbbbbb"},
    std::vector{"", "a", "b", "aa", "bb", "abb", "aab", "aaaaaaaaaaaaabbbbbbbbbbbb", "aaaaaaaaaaaabbbbbbbbbbbbb"},
    true
  );

  tests.emplace_back(
    "even count of b (b^(2 * n), n > 0)",
    GetEvenCountOfBGrammar(),
    std::vector{"bb", "bbbb", "bbbbbb", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"},
    std::vector{"", "b", "ab", "bbb", "bbbbb", "abb", "bba", "bbabb"},
    true
  );

  tests.emplace_back(
    "matching pairs (a^n b^n, n >= 0)",
    GetMatchingPairsWithEmptyGrammar(),
    std::vector{"", "ab", "aabb", "aaabbb", "aaaabbbb", "aaaaaaaaaaaaabbbbbbbbbbbbb"},
    std::vector{"a", "b", "aa", "bb", "abb", "aab", "aaaaaaaaaaaaabbbbbbbbbbbb", "aaaaaaaaaaaabbbbbbbbbbbbb"},
    true
  );

  tests.emplace_back(
    "nothing",
    GetEmptyGrammar(),
    std::vector<const char*>{},
    std::vector{"", "a", "b", "ab", "aa", "ba", "bb", "aaabbbbaababaababsdff", "d"},
    true
  );

  tests.emplace_back(
    "only empty word",
    GetEmptyWordGrammar(),
    std::vector{""},
    std::vector{"a", "b", "ab", "aa", "ba", "bb", "aaabbbbaababaababsdff", "d"},
    true
  );

  tests.emplace_back(
    "only empty word (sophisticated)",
    GetSophisticatedEmptyWordGrammar(),
    std::vector{""},
    std::vector{"a", "b", "ab", "aa", "ba", "bb", "abc", "aaabbbbaababaababsdff", "d"},
    false
  );

  tests.emplace_back(
    "grammar from akhcheck (parentheses)",
    GetAkhcheckGrammar(),
    std::vector{"aababb", "", "ab", "abab", "aabb", "aabbab"},
    std::vector{"aabbba", "a", "b", "aa", "bb", "ba", "aba"},
    true
  );

  tests.emplace_back(
    "buffalo grammar",
    GetBuffaloGrammar(),
    std::vector{"buffalo", "buffalo buffalo", "buffalo buffalo buffalo", "buffalo buffalo buffalo", "buffalo buffalo buffalo buffalo", "buffalo buffalo buffalo buffalo buffalo buffalo buffalo buffalo"},
    std::vector{"", "buffalo ", " buffalo", " ", "buffal", "uffalo", "buffalo buffal", "buffa lo"},
    false
  );

  tests.emplace_back(
    "lr buffalo grammar",
    GetLRBuffaloGrammar(),
    std::vector{"buffalo", "buffalo buffalo", "buffalo buffalo buffalo", "buffalo buffalo buffalo", "buffalo buffalo buffalo buffalo", "buffalo buffalo buffalo buffalo buffalo buffalo buffalo buffalo"},
    std::vector{"", "buffalo ", " buffalo", " ", "buffal", "uffalo", "buffalo buffal", "buffa lo"},
    true
  );

  tests.emplace_back(
    "lorem ipsum",
    GetLoremIpsumGrammar(),
    std::vector{"lorem.", "lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ut enim ad minim veniam quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. excepteur sint occaecat cupidatat non proident sunt in culpa qui officia deserunt mollit anim id est laborum.", "duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.", "lorem ipsum dolor."},
    std::vector{"", "lorem", ". lorem.", "lore.", " ", " ."},
    true
  );

  tests.emplace_back(
    "epsilon_producing grammar",
    GetGrammarWithEpsilonProducing(),
    std::vector{"abcde", "abcd", "abde", "abd", "bcde", "bcd", "bde", "bd"},
    std::vector{"", "b", "d", "bc", "be", "abce", "acde", "abcdef", "aabcde", "abcdee", "abbcde", "abcdea"},
    true
  );

  // clang-format on

  return tests;
}();
