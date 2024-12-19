#include <iostream>

#include "LRParser.h"
#include "RecursiveFunctionsGrammar.h"

int main() {
  std::filesystem::path path =
      "/Users/mihailsimakov/Documents/Programs/CLionProjects/Buffalo/test.bf";

  // getting builders registry
  auto [builders, grammar] = cRecursiveFunctionsGrammar;
  auto builder = LRParserDetails::LRTableBuilder(std::move(grammar));

  builder.save_to(path);
}
