#include <iostream>

#include "EarleyParser.h"
#include "Grammar.h"
#include "GrammarBuilder.h"

using std::cout, std::endl;

static Grammar GetParenthesesGrammar() {
  GrammarBuilder builder;

  // here a is (, b is )
  builder.add_rule('S', "SS");
  builder.add_rule('S', "aSb");
  builder.add_rule('S', "ab");

  return builder.get_grammar();
}

int main() {
  auto grammar = GetParenthesesGrammar();

  auto parser = EarleyParser::fit(grammar);
  cout << parser.predict("ab") << endl;
}
