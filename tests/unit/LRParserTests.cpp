#include <gtest/gtest.h>

#include "LRParser.h"
#include "TestGrammars.h"
#include "fmt/format.h"

TEST(LRParserTests, grammars_test) {
  for (const auto& [name, grammar, acceptible, unacceptable, is_lr] :
       test_grammars) {
    std::cout << "grammar: " << name << std::endl;

    if (!is_lr) {
      ASSERT_THROW({ LRParser::fit(grammar); }, LRParserDetails::ActionsConflictException)
                                    << "Current grammar is not an lr one but "
                                       "constructor hasn't thrown an "
                                       "exception.";
      continue;
    }

    auto parser = LRParser::fit(grammar);

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
