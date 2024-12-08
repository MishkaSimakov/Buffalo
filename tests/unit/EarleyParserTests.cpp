#include <gtest/gtest.h>

#include "EarleyParser.h"
#include "TestGrammars.h"
#include "fmt/format.h"

TEST(EarleyParserTests, grammars_test) {
  for (const auto& [name, grammar, acceptible, unacceptable] : test_grammars) {
    std::cout << "grammar: " << name << std::endl;
    auto parser = EarleyParser::fit(grammar);

    for (const char* word : acceptible) {
      std::cout << word << std::endl;
      ASSERT_TRUE(parser.predict(word))
          << fmt::format("grammar {:?} should accept word {:?}", name, word);
    }

    for (const char* word : unacceptable) {
      std::cout << word << std::endl;
      ASSERT_FALSE(parser.predict(word)) << fmt::format(
          "grammar {:?} should NOT accept word {:?}", name, word);
    }
  }
}
