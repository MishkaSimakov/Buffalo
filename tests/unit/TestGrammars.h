#pragma once

#include <tuple>
#include <vector>

#include "Grammar.h"

using TestGrammarT = std::tuple<const char*, Grammar, std::vector<const char*>,
                                std::vector<const char*>>;

extern const std::vector<TestGrammarT> test_grammars;
