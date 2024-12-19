#include <iostream>

#include "LRParser.h"
#include "RecursiveFunctionsGrammar.h"

int main() {
  std::filesystem::path path =
      "/Users/mihailsimakov/Documents/Programs/CLionProjects/Buffalo/test.bf";

  // getting builders registry
  auto [builders, _] = cRecursiveFunctionsGrammar;

  const char* program = "(extern)print(number);add(x,0)=x;add(x,y+1)=successor(add);print_n_times(x,0)=0;print_n_times(x,n+1)=add(print(x),print_n_times);main()=print_n_times(123,123);";
  auto tokens = Lexing::LexicalAnalyzer::get_tokens(program);

  LRParser parser(path, builders);
  auto node = parser.parse(tokens);

  if (!node) {
    std::cout << "ERROR!" << std::endl;
    return 1;
  }

  PrintSyntaxTreeRecursive("", **node, true);

  return 0;
}
