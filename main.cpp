#include <iostream>

#include "EarleyParser.h"
#include "Grammar.h"
#include "GrammarBuilder.h"

using std::cout, std::endl;


static Grammar GetSophisticatedEmptyWordGrammar() {
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

static Grammar GetEvenPalindromesGrammar() {
  GrammarBuilder builder;

  builder.add_rule('S', "aSa");
  builder.add_rule('S', "bSb");
  builder.add_rule('S', "");

  return builder.get_grammar();
}

int main() {
  auto grammar = GetSophisticatedEmptyWordGrammar();

  auto parser = EarleyParser::fit(grammar);


  cout << parser.predict("") << endl;
}
