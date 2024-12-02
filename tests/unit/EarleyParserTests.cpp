#include <gtest/gtest.h>

#include "EarleyParser.h"
#include "TestGrammars.h"
#include "fmt/format.h"

TEST(EarleyParserTests, grammars_test) {
  for (const auto& [name, grammar, acceptible, unacceptable] : test_grammars) {
    auto parser = EarleyParser::fit(grammar);

    for (const char* word : acceptible) {
      ASSERT_TRUE(parser.predict(word))
          << fmt::format("grammar {:?} should accept word {:?}", name, word);
    }

    for (const char* word : unacceptable) {
      ASSERT_FALSE(parser.predict(word)) << fmt::format(
          "grammar {:?} should NOT accept word {:?}", name, word);
    }
  }
}
